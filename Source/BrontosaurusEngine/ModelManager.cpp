#include "stdafx.h"
#undef new
#include "Engine.h"
#include "ModelManager.h"
#include "ModelLoader.h"
#include "ConstBufferTemplate.h"
#include <ThreadPool.h>
#include "FBXLoader.h"

#include "AnimationController.h"
#include "AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

#define new CARL_NEW

CModelManager::CModelManager()
	: myModelList(64)
{
}

CModelManager::~CModelManager()
{
	for (CModel& model : myModelList)
	{
		if (!model.mySceneAnimators.empty())
		{
			for (auto& garbageNow : model.mySceneAnimators)
			{
				garbageNow.second.Release();
			}
		}
		if (model.myBindposeSceneAnimator)
		{
			model.myBindposeSceneAnimator->Release();
		}
	}

	CFBXLoader::ReleaseScenes();
}

const CModelManager::ModelId CModelManager::LoadModel(const std::string& aModelPath)
{
	ModelId newModelID = -1;

	if (myModels.find(aModelPath) == myModels.end()) //check if derp, else flip bool
	{
		if (myFreeModelIDs.Size() != 0)
		{
			newModelID = myFreeModelIDs.Pop();
		}
		else
		{
			newModelID = myModelList.Size();
			myModelList.Add();
		}

		myModels[aModelPath] = newModelID;

		if (CreateModel(aModelPath, newModelID) == false)
		{
			myModelList.Pop();
			myModels.erase(aModelPath);
			DL_PRINT_WARNING("Failed to load model %s", aModelPath.c_str());
			return NULL_MODEL;
		}

		LoadAnimations(aModelPath, newModelID);
	}
	DL_PRINT("model count: %d", (int)myModelList.Size());
	myModelList[myModels[aModelPath]].AddRef();
	return myModels[aModelPath];
}

CModel* CModelManager::GetModel(const ModelId aModelID)
{
	if (aModelID == -1 || myModelList[aModelID].GetInitialized() == false)
	{
		return nullptr;
	}
	return &myModelList[aModelID];
}
void CModelManager::RemoveModel(const ModelId aModelID)
{
	myModelList[aModelID].RemoveRef();
	
	if (myModelList[aModelID].GetRefCount() <= 0)
	{
		myFreeModelIDs.Add(aModelID);
		if (!myModelList[aModelID].mySceneAnimators.empty())
		{
			for (auto& garbageNow : myModelList[aModelID].mySceneAnimators)
			{
				garbageNow.second.Release();
			}
		}
		myModelList[aModelID] = CModel();
	}
}

int CModelManager::GetModelRefCount(const ModelId aModelID) const
{
	if (aModelID == -1 || myModelList[aModelID].GetInitialized() == false)
	{
		return 0;
	}

	return myModelList[aModelID].GetRefCount();
}

const std::string& CModelManager::GetFilePath(const ModelId aModelID) const
{
	for (const auto& model : myModels)
	{
		if (model.second == aModelID)
		{
			return model.first;
		}
	}

	static const std::string NullString;
	return NullString;
}

void CModelManager::Optimize()
{
	myModelList.Optimize();
}

void CModelManager::LoadAnimations(const std::string& aPath, const ModelId aModelId)
{
	static const std::string Directory("Models/Animations/");
	std::string modelName = aPath;
	modelName -= std::string(".fbx");
	CU::FindAndReplace(modelName, "Meshes", "Animations");
	modelName.insert(Directory.length(), "ANI");
	modelName += std::string("_");

	CModel* mdl = GetModel(aModelId);
	const aiScene* scene = mdl->GetScene();
	
	if (mdl != nullptr && /*scene->HasAnimations()*/mdl->HasBones())
	{
		mdl->myBindposeSceneAnimator = new CSceneAnimator();
		mdl->myBindposeSceneAnimator->Init(scene);

		size_t bindposeBoneCount = mdl->myBindposeSceneAnimator->GetBoneCount();

		mdl->mySceneAnimators.clear();
		CFBXLoader loader;
		for (int i = 0; i < locAnimationState.AnimationStates.Size(); ++i)
		{
			const std::string& animationName = locAnimationState.AnimationStates[i];

			const aiScene* animationScene = loader.GetScene(modelName + animationName + ".fbx");
			
			if (!animationScene)
			{
				continue;
			}

			if (animationScene->mNumMaterials > 1)
			{
				DL_MESSAGE_BOX("More than one material found in animation fbx scene\n%s", aPath.c_str());
			}

			mdl->mySceneAnimators[static_cast<eAnimationState>(i)] = CSceneAnimator();
			mdl->mySceneAnimators[static_cast<eAnimationState>(i)].Init(animationScene);

			if (mdl->mySceneAnimators[static_cast<eAnimationState>(i)].GetBoneCount() != bindposeBoneCount)
			{
				//DL_MESSAGE_BOX("Bone count in animation %s different from bindpose\nFile: %s", animationName.c_str(), aPath.c_str());
			}

			//mdl->mySceneAnimators[animationName].CarlSave(modelName + animationName + ".animation");
			//mdl->mySceneAnimators[animationName].CarlLoad(modelName + animationName + ".animation");
		}

		if (mdl->mySceneAnimators.empty() == true)
		{
			mdl->mySceneAnimators[eAnimationState::idle01] = CSceneAnimator();
			mdl->mySceneAnimators[eAnimationState::idle01].Init(mdl->GetScene());
			//mdl->mySceneAnimators["idle"] = CSceneAnimator();
			//mdl->mySceneAnimators["idle"].Init(mdl->GetScene());
		}

		if (mdl->mySceneAnimators.find(/*"idle"*/eAnimationState::idle01) != mdl->mySceneAnimators.end())
		{
			mdl->mySceneAnimator = &mdl->mySceneAnimators[/*"idle"*/eAnimationState::idle01];
		}
		else if (mdl->mySceneAnimators.begin() != mdl->mySceneAnimators.end())
		{
			mdl->mySceneAnimator = &mdl->mySceneAnimators.begin()->second;
		}
		else
		{
			mdl->mySceneAnimator = nullptr;
		}

		if (mdl->mySceneAnimator)
		{
			if (mdl->mySceneAnimator->CurrentAnimIndex != -1)
			{
				mdl->mySceneAnimator->PlayAnimationForward();
			}
			else
			{
				DL_MESSAGE_BOX("No animation found in %s", aPath.c_str());
				mdl->mySceneAnimator = nullptr;
			}
		}
	}
}

bool CModelManager::CreateModel(const std::string& aModelPath, ModelId aNewModel)
{
	CModel& newModel = myModelList[aNewModel];
	return CModelLoader::LoadModel(aModelPath, newModel);
}

bool CModelManager::DoesModelExists(const std::string& aModelPath)
{
	return myModels.find(aModelPath) != myModels.end();
}

