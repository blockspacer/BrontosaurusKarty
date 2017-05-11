#include "stdafx.h"
#include "BoostPadComponentManager.h"
#include "BoostPadComponent.h"

CBoostPadComponentManager::CBoostPadComponentManager()
{
	myComponents.Init(100);
}


CBoostPadComponentManager::~CBoostPadComponentManager()
{
}

CBoostPadComponent* CBoostPadComponentManager::CreateAndRegisterComponent()
{
	CBoostPadComponent* newComponent = nullptr;
	if(CComponentManager::GetInstancePtr() != nullptr)
	{	
		newComponent = new CBoostPadComponent();
		myComponents.Add(newComponent);
		CComponentManager::GetInstancePtr()->RegisterComponent(newComponent);
	}
	return newComponent;
}

void CBoostPadComponentManager::Update()
{
	for(unsigned int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update();
	}
}