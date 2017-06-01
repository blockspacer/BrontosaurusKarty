#pragma once
#include "ItemTypes.h"
#include "BoostData.h"

namespace Physics
{
	class CPhysicsScene;
}

class CItemWeaponBehaviourComponentManager;
class CGameObjectManager;
class CColliderComponentManager;
class CRedShellManager;
class CBlueShellComponentManager;

struct SItemDrop
{
	eItemTypes myType;
	char myChance;
};

class CItemFactory
{
public:
	CItemFactory();
	~CItemFactory();

	void Init(CGameObjectManager& aGameObjectManager ,CItemWeaponBehaviourComponentManager& aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager, CRedShellManager& aRedShellManager, CBlueShellComponentManager& aBlueShellManager);


	eItemTypes RandomizeItem(CComponent* aPlayerCollider);

	int CreateItem(const eItemTypes aItemType, CComponent* userComponent);

private:
	void CreateBananaBuffer();
	void CreateShellBuffer();
	void CreateRedShellBuffer();
	void CreateBlueShellBuffer();
	void CreatePlacementDrops();

private:
	Physics::CPhysicsScene* myPhysicsScene;
	CItemWeaponBehaviourComponentManager* myItemBeheviourComponentManager;
	CGameObjectManager* myGameObjectManager;
	CColliderComponentManager* myColliderManager;
	CRedShellManager* myRedShellManager;
	CBlueShellComponentManager* myBlueShellManager;

	CU::GrowingArray<CGameObject*> myShells;
	CU::GrowingArray<CGameObject*> myActiveShells;

	CU::GrowingArray<CGameObject*> myRedShells;
	CU::GrowingArray<CGameObject*> myActiveRedShells;

	CU::GrowingArray<CGameObject*> myBlueShells;
	CU::GrowingArray<CGameObject*> myActiveBlueShells;

	CU::GrowingArray<CGameObject*> myBananas;
	CU::GrowingArray<CGameObject*> myActiveBananas;

	SBoostData myStartBoostData;
	SBoostData myLightningBoostData;

	CU::GrowingArray<SBoostData> myLightningBoostBuffer;
	CU::GrowingArray<CU::GrowingArray<SItemDrop>> myPlacementDrops;
};

