#include "stdafx.h"
#include "BlueShellComponentManager.h"
#include "BlueShellBehaviourComponent.h"


CBlueShellComponentManager::CBlueShellComponentManager(CU::GrowingArray<CGameObject*>& aListOfKartObjects)
{
	myKartObjects = &aListOfKartObjects;
	myComponents.Init(10);
}


CBlueShellComponentManager::~CBlueShellComponentManager()
{
}

void CBlueShellComponentManager::Update(const float aDeltaTime)
{
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}

CBlueShellBehaviourComponent * CBlueShellComponentManager::CreateAndRegisterComponent()
{
	CBlueShellBehaviourComponent* blue = new CBlueShellBehaviourComponent(*myKartObjects);

	CComponentManager::GetInstance().RegisterComponent(blue);

	myComponents.Add(blue);

	return blue;
}
