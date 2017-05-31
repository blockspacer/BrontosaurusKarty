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

	void Init(CGameObjectManager& aGameObjectManager ,CItemWeaponBehaviourComponentManager& aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager, CRedShellManager& aRedShellManager);


	eItemTypes RandomizeItem(CComponent* aPlayerCollider);

	int CreateItem(const eItemTypes aItemType, CComponent* userComponent);

private:
	void CreateBananaBuffer();
	void CreateShellBuffer();
	void CreateRedShellBuffer();
	void CreatePlacementDrops();

private:
	Physics::CPhysicsScene* myPhysicsScene;
	CItemWeaponBehaviourComponentManager* myItemBeheviourComponentManager;
	CGameObjectManager* myGameObjectManager;
	CColliderComponentManager* myColliderManager;
	CRedShellManager* myRedShellManager;

	CU::GrowingArray<CGameObject*> myShells;
	CU::GrowingArray<CGameObject*> myActiveShells;

	CU::GrowingArray<CGameObject*> myRedShells;
	CU::GrowingArray<CGameObject*> myActiveRedShells;

	CU::GrowingArray<CGameObject*> myBananas;
	CU::GrowingArray<CGameObject*> myActiveBananas;

	SBoostData myStartBoostData;


	CU::GrowingArray<CU::GrowingArray<SItemDrop>> myPlacementDrops;
};

