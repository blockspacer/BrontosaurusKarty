#include "stdafx.h"
#include "KartControllerComponentManager.h"
#include "KartControllerComponent.h"


CKartControllerComponentManager::CKartControllerComponentManager()
{
	myComponents.Init(4);
	myShouldUpdate = true;
}


CKartControllerComponentManager::~CKartControllerComponentManager()
{
}

CKartControllerComponent * CKartControllerComponentManager::CreateAndRegisterComponent()
{
	CKartControllerComponent* kartController = new CKartControllerComponent();
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

void CKartControllerComponentManager::Init(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}
