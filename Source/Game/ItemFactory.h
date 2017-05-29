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

class CItemFactory
{
public:
	CItemFactory();
	~CItemFactory();

	void Init(CGameObjectManager& aGameObjectManager ,CItemWeaponBehaviourComponentManager& aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager, CRedShellManager& aRedShellManager);


	eItemTypes RandomizeItem();

	int CreateItem(const eItemTypes aItemType, CComponent* userComponent);

private:
	void CreateBananaBuffer();
	void CreateShellBuffer();
	void CreateRedShellBuffer();

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

};

