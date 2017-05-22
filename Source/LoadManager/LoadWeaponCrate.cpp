#include "stdafx.h"
#include "LoadWeaponCrate.h"
#include "Components/WeaponCratePickupComponent.h"
#include "Components/PickupComponentManager.h"
#include "Game/ItemFactory.h"

int LoadWeapoCrateComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CItemFactory* ItemFactory = loadManager.GetCurrentPLaystate().GetItemFactory();
	CItemPickupComponent* pickup = CPickupComponentManager::GetInstance()->CreateWeaponCratePickupComponent(2, *ItemFactory);
	return pickup->GetId();
}
