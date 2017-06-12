#include "stdafx.h"
#include "KartControllerComponentManager.h"
#include "KartControllerComponent.h"
#include "../Game/NavigationSpline.h"
#include "SParticipant.h"

#include "..\ThreadedPostmaster/RaceStartedMessage.h"


CKartControllerComponentManager::CKartControllerComponentManager(): myPhysicsScene(nullptr)
{
	myComponents.Init(4);
	myShouldUpdate = false;
}

CKartControllerComponentManager::~CKartControllerComponentManager()
{
	POSTMASTER.Unsubscribe(this);
}

CKartControllerComponent * CKartControllerComponentManager::CreateAndRegisterComponent(CModelComponent& aModelComponent, const short aControllerIndex)
{
	CKartControllerComponent* kartController = new CKartControllerComponent(this, aModelComponent, aControllerIndex);
	kartController->Init(myPhysicsScene);
	CComponentManager::GetInstance().RegisterComponent(kartController);
	myComponents.Add(kartController);
	return kartController;
}

CKartControllerComponent * CKartControllerComponentManager::CreateAndRegisterComponent(CModelComponent & aModelComponent, const SParticipant & aParticipant)
{
	CKartControllerComponent* kartController = new CKartControllerComponent(this, aModelComponent, static_cast<short>(aParticipant.myInputDevice), static_cast<short>(aParticipant.mySelectedCharacter));
	kartController->Init(myPhysicsScene);
	CComponentManager::GetInstance().RegisterComponent(kartController);
	myComponents.Add(kartController);
	return kartController;
}

void CKartControllerComponentManager::Update(const float aDeltaTime)
{
	static bool lastShouldUpdate = false;
	if (myShouldUpdate == false)
	{
		for (int i = 0; i < myComponents.Size(); i++)
		{
			myComponents[i]->CountDownUpdate(aDeltaTime);
		}
		return;
	}
	if (myShouldUpdate != lastShouldUpdate)
	{
		if (myShouldUpdate == true)
		{
			for (int i = 0; i < myComponents.Size(); i++)
			{
				myComponents[i]->ApplyStartBoost();
			}
		}
		lastShouldUpdate = myShouldUpdate;
	}
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
#if RENDER_SPLINE == 1
	Render();
#endif
}

//void CKartControllerComponentManager::ShouldUpdate(const bool aShouldUpdate)
//{
//	myShouldUpdate = aShouldUpdate;
//}

void CKartControllerComponentManager::LoadNavigationSpline(const CU::CJsonValue& aJsonValue)
{
	myNavigationSpline.LoadFromJson(aJsonValue);
}

const CNavigationSpline& CKartControllerComponentManager::GetNavigationSpline() const
{
	return myNavigationSpline;
}

void CKartControllerComponentManager::Init()
{
	myNavigationSpline.SetDistancesToGoal(myGoalComponentPointer);
}

void CKartControllerComponentManager::SetPhysiscsScene(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

const CU::Vector3f CKartControllerComponentManager::GetClosestSpinesDirection(const CU::Vector3f& aKartPosition)
{
	float distance = 999999;
	unsigned int index = -1;
	
	for(unsigned int i = 0; i < myNavigationSpline.GetNumberOfPoints(); i++)
	{
		float newDistance = CU::Vector2f(myNavigationSpline.GetPoint(i).myPosition - CU::Vector2f(aKartPosition.x, aKartPosition.z)).Length2();
		if(newDistance < distance)
		{
			distance = newDistance;
			index = i;
		}
	}

	if(index > myNavigationSpline.GetNumberOfPoints())
	{
		return CU::Vector3f();
	}
	CU::Vector3f direction;
	direction.x = myNavigationSpline.GetPoint(index).myForwardDirection.x;
	direction.y = 0.0f;
	direction.z = myNavigationSpline.GetPoint(index).myForwardDirection.y;
	return direction;
}

const int CKartControllerComponentManager::GetClosestSpinesIndex(const CU::Vector3f& aKartPosition)
{
	float distance = 999999;
	unsigned int index = -1;

	for (unsigned int i = 0; i < myNavigationSpline.GetNumberOfPoints(); i++)
	{
		float newDistance = CU::Vector2f(myNavigationSpline.GetPoint(i).myPosition - CU::Vector2f(aKartPosition.x, aKartPosition.z)).Length2();
		if (newDistance < distance)
		{
			distance = newDistance;
			index = i;
		}
	}

	if (index > myNavigationSpline.GetNumberOfPoints())
	{
		return 0;
	}

	return index;
}

const SNavigationPoint* CKartControllerComponentManager::GetNavigationPoint(const int aIndex)
{
	if(aIndex < myNavigationSpline.GetNumberOfPoints())
	{
		return &myNavigationSpline.GetPoint(aIndex);
	}
	return nullptr;
}

eMessageReturn CKartControllerComponentManager::DoEvent(const CRaceStartedMessage& aMessage)
{
	myShouldUpdate = true;
	return eMessageReturn::eContinue;
}


#if RENDER_SPLINE == 1
#include "Engine.h"
#include "ModelManager.h"

#include "RenderMessages.h"
#include "Renderer.h"

#include "../Physics/CollisionLayers.h"
#include "../Physics/PhysicsScene.h"
void CKartControllerComponentManager::Render()
{
	

	CModelManager* modelManager = MODELMGR;
	CModelManager::ModelId model = MODELMGR->LoadModel("Models/Meshes/M_ColorCube.fbx");

	for(int i = 0; i < myNavigationSpline.GetNumberOfPoints(); ++i)
	{
		const SNavigationPoint& point = myNavigationSpline.GetPoint(i);

		SDeferredRenderModelParams params;

		CU::Vector3f pos(point.myPosition.x, 0.f, point.myPosition.y);

		Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(pos, -CU::Vector3f::UnitY, 50, Physics::eGround);

		if(raycastHitData.hit == false)
		{
			raycastHitData = myPhysicsScene->Raycast(pos, CU::Vector3f::UnitY, 50, Physics::eGround);

			if (raycastHitData.hit == true)
			{

				pos.y += raycastHitData.distance;
			}
		}
		else
		{
			pos.y -= raycastHitData.distance;
		}

		params.myTransform.SetPosition(pos);
		params.myTransformLastFrame = params.myTransform;
		params.myRenderToDepth = false;
		
		SRenderMessage* msg = nullptr;

		SRenderModelInstancedMessage* instancedMsg = new SRenderModelInstancedMessage();
		instancedMsg->myModelID = model;
		instancedMsg->myRenderParams = params;
		msg = instancedMsg;


		RENDERER.AddRenderMessage(msg);
	}
	
}
#endif
