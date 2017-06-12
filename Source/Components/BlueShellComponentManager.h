#pragma once

class CBlueShellBehaviourComponent;
class CGameObjectManager;
class CExplosionComponentManager;
class CColliderComponentManager;

class CBlueShellComponentManager
{
public:
	CBlueShellComponentManager(CU::GrowingArray<CGameObject*>& aListOfKartObjects, CGameObjectManager* aGameObjectManager, CExplosionComponentManager* aExplosionManager, CColliderComponentManager* aColliderManager);
	~CBlueShellComponentManager();

	void Update(const float aDeltaTime);

	CBlueShellBehaviourComponent* CreateAndRegisterComponent();

private:

	CU::GrowingArray<CBlueShellBehaviourComponent*> myComponents;

	CGameObjectManager* myGameObjectManager;
	CExplosionComponentManager* myExplosionManager;
	CColliderComponentManager* myColliderManager;

	CU::GrowingArray<CGameObject*>* myKartObjects;
};

