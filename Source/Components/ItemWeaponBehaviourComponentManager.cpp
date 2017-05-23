#include "stdafx.h"
#include "ItemWeaponBehaviourComponentManager.h"
#include "ItemWeaponBehaviourComponent.h"


CItemWeaponBehaviourComponentManager::CItemWeaponBehaviourComponentManager()
{
	myComponents.Init(30);
}


CItemWeaponBehaviourComponentManager::~CItemWeaponBehaviourComponentManager()
{
}

void CItemWeaponBehaviourComponentManager::Init(Physics::CPhysicsScene * aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

CItemWeaponBehaviourComponent * CItemWeaponBehaviourComponentManager::CreateAndRegisterComponent()
{
	CItemWeaponBehaviourComponent* itemBeheviour = new CItemWeaponBehaviourComponent();

	itemBeheviour->Init(myPhysicsScene);

	CComponentManager::GetInstance().RegisterComponent(itemBeheviour);

	myComponents.Add(itemBeheviour);

	return itemBeheviour;
}

void CItemWeaponBehaviourComponentManager::Update(const float aDeltatime)
{
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltatime);
	}
}
