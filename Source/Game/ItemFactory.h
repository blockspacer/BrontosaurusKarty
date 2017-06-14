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
class CScene;

struct SHeldItem
{
	CGameObject* item;
	CGameObject* holder;
};

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

	void Init(CGameObjectManager& aGameObjectManager ,CItemWeaponBehaviourComponentManager& aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager, CRedShellManager& aRedShellManager, CBlueShellComponentManager& aBlueShellManager, CScene* aScene);


	eItemTypes RandomizeItem(CComponent* aPlayerCollider);

	int CreateItem(const eItemTypes aItemType, CComponent* userComponent);

	void ReleaseItem(CComponent* userComponent, CU::Vector2f aInput = CU::Vector2f::Zero);

	void Update();

private:
	void CreateBananaBuffer();
	void CreateShellBuffer();
	void CreateRedShellBuffer();
	void CreateBlueShellBuffer();
	void CreatePlacementDrops();
	eItemTypes CheckItem(std::string  aItem);

private:
	Physics::CPhysicsScene* myPhysicsScene;
	CItemWeaponBehaviourComponentManager* myItemBeheviourComponentManager;
	CGameObjectManager* myGameObjectManager;
	CColliderComponentManager* myColliderManager;
	CRedShellManager* myRedShellManager;
	CBlueShellComponentManager* myBlueShellManager;
	CScene* myScene;

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
	float	myLightningTimeModifier;

	CU::GrowingArray<SBoostData> myLightningBoostBuffer;
	CU::GrowingArray<CU::GrowingArray<SItemDrop>> myPlacementDrops;


	CU::GrowingArray<SHeldItem> myHeldBananas;
	CU::GrowingArray<SHeldItem> myHeldGreenShells;
	CU::GrowingArray<SHeldItem> myHeldRedShells;
};

