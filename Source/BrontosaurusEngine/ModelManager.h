#pragma once
#include "Model.h"

namespace CU
{
	class ThreadPool;
	class DynamicString;
}

class CModel;
class CSprite;
class CModelInstance;
class CSpriteInstance;

#define NULL_MODEL -1

class CModelManager
{
public:
	typedef int ModelId;
	
	CModelManager();
	~CModelManager();

	const ModelId LoadModel(const std::string& aModelPath);
	CModel* GetModel(const ModelId aModelID);
	void RemoveModel(const ModelId aModelID);
	int GetModelRefCount(const ModelId aModelID) const;
	const std::string& GetFilePath(const ModelId aModelID) const;

	void Optimize();

private:
	void LoadAnimations(const std::string& aPath, const ModelId aModelId);
	bool CreateModel(const std::string& aModelPath, ModelId aNewModel);
	bool DoesModelExists(const std::string& aModelPath);

private:
	std::unordered_map<std::string, ModelId> myModels;
	CU::GrowingArray<ModelId> myFreeModelIDs;
	CU::GrowingArray<CModel, ModelId> myModelList;
};

