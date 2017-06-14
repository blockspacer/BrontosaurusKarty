#include "stdafx.h"
#include "PickupComponentManager.h"
#include "WeaponCratePickupComponent.h"
#include "DecalComponent.h"
#include "GameObjectManager.h"

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



void CPickupComponentManager::AddDecalsToItemsboxes(CScene& aScene, CGameObjectManager* gameObjectManager)
{
	for (std::pair<const int, IPickupComponent*> comp : myWeaponCrates)
	{
		CDecalComponent* decal = new CDecalComponent(aScene);
		CComponentManager::GetInstance().RegisterComponent(decal);
		decal->SetDecalIndex(6);
		
		CGameObject* decalHolder = gameObjectManager->CreateGameObject();
		decalHolder->GetLocalTransform().myPosition.Set(0.0f, -2.f, 0.0f);
		decalHolder->GetLocalTransform().SetScale({ 3.0f, 4.f, 3.f });
		decalHolder->GetLocalTransform().RotateAroundAxis(3.141592f, CU::Axees::Y);
		decalHolder->AddComponent(decal);

		comp.second->GetParent()->AddComponent(decalHolder);
	}
}

CPickupComponentManager::CPickupComponentManager()
{
}

CPickupComponentManager::~CPickupComponentManager()
{
}
