#pragma once

class CWeaponPickupComponent;

class CWeaponCratePickupComponent;

class CPickupComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CPickupComponentManager* GetInstance();

	CWeaponCratePickupComponent* CreateWeaponCratePickupComponent(const float aRespawnTime);
	CWeaponCratePickupComponent* GetWeaponCratePickupComponent(const int aId);
	void DeactivateWeaponCratePickup(const int aId);

	void Update(const float aDeltaTime);
private:
	std::map<int, CWeaponCratePickupComponent*> myWeaponCrates;

	static CPickupComponentManager* ourInstance;
	CPickupComponentManager();
	~CPickupComponentManager();
};

