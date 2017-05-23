#pragma once
#include "ItemTypes.h"

namespace Physics
{
	class CPhysicsScene;
}

class CItemWeaponBehaviourComponentManager;
class CGameObjectManager;
class CColliderComponentManager;

class CItemFactory
{
public:
	CItemFactory();
	~CItemFactory();

	void Init(CGameObjectManager& aGameObjectManager ,CItemWeaponBehaviourComponentManager& aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager);


	eItemTypes RandomizeItem();

	int CreateItem(const eItemTypes aItemType, CComponent* userComponent);

private:
	void CreateBananaBuffer();
	void CreateShellBuffer();

private:
	Physics::CPhysicsScene* myPhysicsScene;
	CItemWeaponBehaviourComponentManager* myItemBeheviourComponentManager;
	CGameObjectManager* myGameObjectManager;
	CColliderComponentManager* myColliderManager;

	CU::GrowingArray<CGameObject*> myShells;
	CU::GrowingArray<CGameObject*> myActiveShells;

	CU::GrowingArray<CGameObject*> myBananas;
	CU::GrowingArray<CGameObject*> myActiveBananas;

};

