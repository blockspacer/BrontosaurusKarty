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
	myActiveShells.Init(25);
	myShells.Init(25);
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

	CreateShellBuffer();
}

void CItemFactory::CreateBananaBuffer()
{
}

void CItemFactory::CreateShellBuffer()
{
	for (int i = 0; i < 25; i++)
	{
		CGameObject* shell = myGameObjectManager->CreateGameObject();
		std::string name = "shell ";
		name += std::to_string(i);
		shell->SetName(name.c_str());

		CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_shell_green_01.fbx");
		shell->AddComponent(model);

			CItemWeaponBehaviourComponent* beheviour = myItemBeheviourComponentManager->CreateAndRegisterComponent();
			shell->AddComponent(beheviour);

		CHazardComponent* hazardous = new CHazardComponent;
		CComponentManager::GetInstance().RegisterComponent(hazardous);
		shell->AddComponent(hazardous);

		//adds collider
		SBoxColliderData crystalMeshColliderData;
		crystalMeshColliderData.IsTrigger = true;
		crystalMeshColliderData.myLayer = Physics::eHazzard;
		crystalMeshColliderData.myCollideAgainst = Physics::GetCollideAgainst(crystalMeshColliderData.myLayer);
		crystalMeshColliderData.material.aDynamicFriction = 0.5f;
		crystalMeshColliderData.material.aRestitution = 0.5f;
		crystalMeshColliderData.material.aStaticFriction = 0.5f;
		crystalMeshColliderData.center.y = 0.5f;
		crystalMeshColliderData.myHalfExtent = CU::Vector3f(0.5f, 0.5f, 0.5f);
		CColliderComponent* shellColliderComponent = myColliderManager->CreateComponent(&crystalMeshColliderData, shell->GetId());
		//CGameObject* colliderObject = myGameObjectManager->CreateGameObject();
		CU::Vector3f offset = shell->GetWorldPosition();

		SRigidBodyData rigidbodah;
		rigidbodah.isKinematic = true;
		rigidbodah.useGravity = false;
		rigidbodah.myLayer = Physics::eHazzard;
		rigidbodah.myCollideAgainst = Physics::GetCollideAgainst(rigidbodah.myLayer);

		CColliderComponent* rigidComponent = myColliderManager->CreateComponent(&rigidbodah, shell->GetId());
		//	colliderObject->SetWorldPosition({ offset.x, offset.y + 0.1f, offset.z });
		shell->AddComponent(shellColliderComponent);
		shell->AddComponent(rigidComponent);

		//shell->AddComponent(colliderObject);
		//collider added

		myShells.Add(shell);
	}
}

eItemTypes CItemFactory::RandomizeItem()
{
	return eItemTypes::eGreenShell;
}

int CItemFactory::CreateItem(const eItemTypes aItemType, CComponent* userComponent)
{
	switch (aItemType)
	{
	case eItemTypes::eGreenShell:
	{
		if (myShells.Size() <= 0)
		{
			myShells.Add(myActiveShells.GetFirst());
			myActiveShells.Remove(myActiveShells.GetFirst());
		}

		CGameObject* shell = myShells.GetLast();
		myShells.Remove(shell);
		shell->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		myActiveShells.Add(shell);
		CU::Matrix44f transform = userComponent->GetParent()->GetToWorldTransform();
		CU::Vector3f position = userComponent->GetParent()->GetWorldPosition();
		shell->GetLocalTransform() = transform;
		shell->SetWorldPosition(position);
		//CU::Vector3f forward = userComponent->GetParent()->GetToWorldTransform().myForwardVector;
		//forward  *=3;
		shell->Move(CU::Vector3f(0,-0.5f,3));
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
