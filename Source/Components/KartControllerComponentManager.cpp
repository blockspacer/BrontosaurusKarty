#include "stdafx.h"
#include "KartControllerComponentManager.h"
#include "KartControllerComponent.h"
#include "PollingStation.h"

CKartControllerComponentManager::CKartControllerComponentManager(): myPhysicsScene(nullptr)
{
	myComponents.Init(4);
	myShouldUpdate = false;
}


CKartControllerComponentManager::~CKartControllerComponentManager()
{
}

CKartControllerComponent * CKartControllerComponentManager::CreateAndRegisterComponent(CModelComponent& aModelComponent, const short aControllerIndex)
{
	CKartControllerComponent* kartController = new CKartControllerComponent(this, aModelComponent, aControllerIndex);
	kartController->Init(myPhysicsScene);
	CComponentManager::GetInstance().RegisterComponent(kartController);
	myComponents.Add(kartController);
	return kartController;
}

void CKartControllerComponentManager::Update(const float aDeltaTime)
{
	if (myShouldUpdate == false)
			return;

	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}

void CKartControllerComponentManager::ShouldUpdate(const bool aShouldUpdate)
{
	myShouldUpdate = aShouldUpdate;
}

void CKartControllerComponentManager::LoadNavigationSpline(const CU::CJsonValue& aJsonValue)
{
	myNavigationSpline.LoadFromJson(aJsonValue);
}

const CNavigationSpline& CKartControllerComponentManager::GetNavigationSpline() const
{
	return myNavigationSpline;
}

void CKartControllerComponentManager::Init(Physics::CPhysicsScene* aPhysicsScene)
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
