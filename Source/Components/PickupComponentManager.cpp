#include "stdafx.h"
#include "PickupComponentManager.h"
#include "WeaponCratePickupComponent.h"

CPickupComponentManager* CPickupComponentManager::ourInstance = nullptr;

void CPickupComponentManager::Create()
{
	if(ourInstance == nullptr)
	{
		ourInstance = new CPickupComponentManager();
	}
}

void CPickupComponentManager::Destroy()
{
	if(ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

CPickupComponentManager * CPickupComponentManager::GetInstance()
{
	return ourInstance;
}

CItemPickupComponent * CPickupComponentManager::CreateWeaponCratePickupComponent(const float aRespawnTime, CItemFactory& aItemFactory)
{
	static int weaponCrateID = 0;
	CItemPickupComponent* cratePickup = new CItemPickupComponent(aItemFactory);
	cratePickup->SetRespawnTime(aRespawnTime);
	cratePickup->SetNetworkId(weaponCrateID);
	COMPMGR.RegisterComponent(cratePickup);
	myWeaponCrates.emplace(weaponCrateID, cratePickup);
	++weaponCrateID;
	return cratePickup;
}

CItemPickupComponent * CPickupComponentManager::GetWeaponCratePickupComponent(const int aId)
{
	return myWeaponCrates.at(aId);
}

void CPickupComponentManager::DeactivateWeaponCratePickup(const int aId)
{
}


void CPickupComponentManager::Update(const float aDeltaTime)
{
	for (std::pair<const int, IPickupComponent*> comp : myWeaponCrates)
	{
		comp.second->Update(aDeltaTime);
	}
}



CPickupComponentManager::CPickupComponentManager()
{
}

CPickupComponentManager::~CPickupComponentManager()
{
}
