#include "stdafx.h"
#include "LoadWeaponCrate.h"
#include "Components/WeaponCratePickupComponent.h"
#include "Components/PickupComponentManager.h"

int LoadWeapoCrateComponent(KLoader::SLoadedComponentData someData)
{
	CItemPickupComponent* pickup = CPickupComponentManager::GetInstance()->CreateWeaponCratePickupComponent(someData.myData.at("RespawnTime").GetFloat());
	return pickup->GetId();
}
