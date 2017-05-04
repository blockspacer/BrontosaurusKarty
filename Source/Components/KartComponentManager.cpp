#include "stdafx.h"
#include "KartComponentManager.h"
#include "KartComponent.h"

CKartComponentManager::CKartComponentManager()
{
	myComponents.Init(16);
}


CKartComponentManager::~CKartComponentManager()
{
}

CKartComponent* CKartComponentManager::CreateComponent()
{
	if(CComponentManager::GetInstancePtr() != nullptr)
	{
		myComponents.Add(new CKartComponent());
		CComponentManager::GetInstancePtr()->RegisterComponent(myComponents.GetLast());
		return myComponents.GetLast();
	}
	else
	{
		DL_ASSERT("COmponent Manager is nullptr");
		return nullptr;
	}
}

void CKartComponentManager::Update(float aDeltaTime)
{
	for(unsigned short i = 0; i < myComponents.Size(); i++)
	{
	
		myComponents[i]->Update(aDeltaTime);
	}
}