#include "stdafx.h"
#include "RedShellManager.h"
#include "RedShellBehaviourComponent.h"


CRedShellManager::CRedShellManager()
{
	myComponents.Init(26);
}


CRedShellManager::~CRedShellManager()
{
}

void CRedShellManager::Init(Physics::CPhysicsScene * aPhysicsScene, CKartControllerComponentManager * aKartManager, CU::GrowingArray<CGameObject*>& aListOfKarts)
{
	myPhysicsScene = aPhysicsScene;
	myKartManager = aKartManager;
	myListOfKarts = &aListOfKarts;
}

void CRedShellManager::Update(const float aDeltatime)
{
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltatime);
	}
}

CU::GrowingArray<CGameObject*> CRedShellManager::GetKarts()
{
	return *myListOfKarts;
}

CRedShellBehaviourComponent * CRedShellManager::CreateAndRegisterComponent()
{
	CRedShellBehaviourComponent* redShell = new CRedShellBehaviourComponent();
	redShell->Init(myPhysicsScene, myKartManager,*myListOfKarts);
	
	myComponents.Add(redShell);

	CComponentManager::GetInstance().RegisterComponent(redShell);

	return redShell;
}


