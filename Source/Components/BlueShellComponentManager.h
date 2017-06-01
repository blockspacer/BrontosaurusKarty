#pragma once

class CBlueShellBehaviourComponent;

class CBlueShellComponentManager
{
public:
	CBlueShellComponentManager(CU::GrowingArray<CGameObject*>& aListOfKartObjects);
	~CBlueShellComponentManager();

	void Update(const float aDeltaTime);

	CBlueShellBehaviourComponent* CreateAndRegisterComponent();

private:

	CU::GrowingArray<CBlueShellBehaviourComponent*> myComponents;

	CU::GrowingArray<CGameObject*>* myKartObjects;
};

