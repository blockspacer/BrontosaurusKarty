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

void CRedShellManager::Init(Physics::CPhysicsScene * aPhysicsScene, CKartControllerComponentManager * aKartManager)
{
	myPhysicsScene = aPhysicsScene;
	myKartManager = aKartManager;
}

void CRedShellManager::Update(const float aDeltatime)
{
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltatime);
	}
}

CRedShellBehaviourComponent * CRedShellManager::CreateAndRegisterComponent()
{
	CRedShellBehaviourComponent* redShell = new CRedShellBehaviourComponent();
	redShell->Init(myPhysicsScene, myKartManager);
	
	myComponents.Add(redShell);

	CComponentManager::GetInstance().RegisterComponent(redShell);

	return redShell;
}


