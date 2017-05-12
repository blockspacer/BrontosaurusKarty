#pragma once

class CWeaponPickupComponent;

class CItemPickupComponent;

class CItemFactory;

class CPickupComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CPickupComponentManager* GetInstance();

	CItemPickupComponent* CreateWeaponCratePickupComponent(const float aRespawnTime, CItemFactory& aItemFactory);
	CItemPickupComponent* GetWeaponCratePickupComponent(const int aId);
	void DeactivateWeaponCratePickup(const int aId);

	void Update(const float aDeltaTime);
private:
	std::map<int, CItemPickupComponent*> myWeaponCrates;

	static CPickupComponentManager* ourInstance;
	CPickupComponentManager();
	~CPickupComponentManager();
};

