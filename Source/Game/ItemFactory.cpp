#include "stdafx.h"
#include "ItemFactory.h"
#include "Components/SpeedHandlerManager.h"
#include "../Physics/PhysicsScene.h"
#include "GameObjectManager.h"
#include "ItemWeaponBehaviourComponentManager.h"
#include "ColliderComponentManager.h"

#include "ItemWeaponBehaviourComponent.h"
#include "HazardComponent.h"
#include "ModelComponent.h"

#include "ModelComponentManager.h"
#include "ConcaveMeshCollider.h"


CItemFactory::CItemFactory()
{
}


CItemFactory::~CItemFactory()
{
}

void CItemFactory::Init(CGameObjectManager& aGameObjectManager, CItemWeaponBehaviourComponentManager & aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager)
{
	myItemBeheviourComponentManager = &aManager;
	myPhysicsScene = aPhysicsScene;
	myGameObjectManager = &aGameObjectManager;
	myColliderManager = &aColliderManager;
}

void CItemFactory::CreateBananaBuffer()
{
}

void CItemFactory::CreateShellBuffer()
{
	for (int i = 0; i < 25; i++)
	{
		CGameObject* shell = myGameObjectManager->CreateGameObject();

		CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_shell_green_01.fbx");

		CItemWeaponBehaviourComponent* beheviour = myItemBeheviourComponentManager->CreateAndRegisterComponent();

		CHazardComponent* hazardous = new CHazardComponent;
		CComponentManager::GetInstance().RegisterComponent(hazardous);

		//adds collider
		SBoxColliderData box;
		box.myHalfExtent = CU::Vector3f(1.0f, 1.0f, 1.0f);
		box.center.y = 1.05f;
		SConcaveMeshColliderData crystalMeshColliderData;
		crystalMeshColliderData.IsTrigger = false;
		crystalMeshColliderData.myPath = "Models/Meshes/M_shell_green_01.fbx";
		crystalMeshColliderData.material.aDynamicFriction = 0.5f;
		crystalMeshColliderData.material.aRestitution = 0.5f;
		crystalMeshColliderData.material.aStaticFriction = 0.5f;
		crystalMeshColliderData.myLayer;
		CColliderComponent* shellColliderComponent = myColliderManager->CreateComponent(&box, shell->GetId());
		CGameObject* colliderObject = myGameObjectManager->CreateGameObject();
		CU::Vector3f offset = shell->GetWorldPosition();

		SRigidBodyData rigidbodah;
		rigidbodah.isKinematic = true;
		rigidbodah.useGravity = false;
		CColliderComponent* rigidComponent = myColliderManager->CreateComponent(&rigidbodah, shell->GetId());
		//	colliderObject->SetWorldPosition({ offset.x, offset.y + 0.1f, offset.z });
		colliderObject->AddComponent(shellColliderComponent);

		colliderObject->AddComponent(rigidComponent);
		shell->AddComponent(colliderObject);
		//collider added

		myShells.Add(shell);
	}
}

eItemTypes CItemFactory::RandomizeItem()
{
	return eItemTypes::eMushroom;
}

int CItemFactory::CreateItem(const eItemTypes aItemType, CComponent* userComponent)
{
	switch (aItemType)
	{
	case eItemTypes::eGreenShell:
		{
		CGameObject* shell = myShells.GetLast();
		shell->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		myActiveShells.Add(shell);
		break;
		}
		
	case eItemTypes::eRedShell:
		//Create shell that homes in on the player in front of the user
		break;
	case eItemTypes::eBlueShell:
		//Create shell that homes in on the leader
		break;
	case eItemTypes::eMushroom:
	{
		SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPowerUp"));
		userComponent->GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		//userComponent->GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost,)
		break;
	}
	case eItemTypes::eGoldenMushroom:
		//boost and have some form of cooldown before 
		break;
	case eItemTypes::eStar:
		//make invincible and boost
		break;
	case eItemTypes::eBulletBill:
		//activate AI controller on player and make invincible and boost
		break;
	case eItemTypes::eLightning:
		//postmaster blast all
		break;
	case eItemTypes::eBanana:
		//create banana
		break;
	default:
		break;
	}

	return 0;
}
