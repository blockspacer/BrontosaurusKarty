#include "stdafx.h"
#include "KartControllerComponentManager.h"
#include "KartControllerComponent.h"


CKartControllerComponentManager::CKartControllerComponentManager()
{
	myComponents.Init(4);
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
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}

void CKartControllerComponentManager::Init(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}
