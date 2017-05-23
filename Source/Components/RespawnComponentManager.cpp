#include "stdafx.h"
#include "RespawnComponentManager.h"
#include "RespawnerComponent.h"

CRespawnComponentManager::CRespawnComponentManager()
{
	myComponents.Init(16);
}


CRespawnComponentManager::~CRespawnComponentManager()
{
}

void CRespawnComponentManager::Update(float aDeltaTime)
{
	for(unsigned int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}
CRespawnerComponent* CRespawnComponentManager::CreateAndRegisterComponent()
{
	CRespawnerComponent* component = nullptr;

	if(CComponentManager::GetInstancePtr() != nullptr)
	{
		component = new CRespawnerComponent();
		myComponents.Add(component);
		CComponentManager::GetInstancePtr()->RegisterComponent(myComponents.GetLast());
	}

	return component;
}