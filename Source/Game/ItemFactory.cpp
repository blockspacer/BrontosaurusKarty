#include "stdafx.h"
#include "ItemFactory.h"
#include "Components/SpeedHandlerManager.h"
#include "../Physics/PhysicsScene.h"
#include "GameObjectManager.h"
#include "ItemWeaponBehaviourComponentManager.h"
#include "RedShellManager.h"
#include "ColliderComponentManager.h"

#include "ItemWeaponBehaviourComponent.h"
#include "RedShellBehaviourComponent.h"
#include "HazardComponent.h"
#include "ModelComponent.h"
#include "ParticleEmitterComponent.h"
#include "ParticleEmitterComponentManager.h"

#include "ModelComponentManager.h"
#include "ConcaveMeshCollider.h"

#include "CommonUtilities/JsonValue.h"


CItemFactory::CItemFactory()
{
	myActiveShells.Init(25);
	myShells.Init(25);

	myBananas.Init(50);
	myActiveBananas.Init(50);

	myRedShells.Init(26);
	myActiveRedShells.Init(26);

	CU::CJsonValue boostList;
	std::string filePath = "Json/Items.json";
	const std::string& errorString = boostList.Parse(filePath);
	CU::CJsonValue levelsArray = boostList.at("Items");
	CU::CJsonValue Item = levelsArray.at("Star");

	myStartBoostData.duration = Item.at("Duration").GetFloat();
	myStartBoostData.accerationBoost = Item.at("AccelerationPercentModifier").GetFloat()/100.0f;
	myStartBoostData.maxSpeedBoost = Item.at("MaxSpeedPercentModifier").GetFloat()/100.0f;
}


CItemFactory::~CItemFactory()
{
}

void CItemFactory::Init(CGameObjectManager& aGameObjectManager, CItemWeaponBehaviourComponentManager & aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager, CRedShellManager& aRedShellManager)
{
	myItemBeheviourComponentManager = &aManager;
	myPhysicsScene = aPhysicsScene;
	myGameObjectManager = &aGameObjectManager;
	myColliderManager = &aColliderManager;
	myRedShellManager = &aRedShellManager;

	CreateShellBuffer();
	CreateBananaBuffer();
	CreateRedShellBuffer();
}

void CItemFactory::CreateBananaBuffer()
{
	for (int i = 0; i < 25; i++)
	{
		CGameObject* banana = myGameObjectManager->CreateGameObject();
		std::string name = "banana ";
		name += std::to_string(i);
		banana->SetName(name.c_str());

		CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Banana_01.fbx");
		model->FlipVisibility();
		banana->AddComponent(model);

		CItemWeaponBehaviourComponent* physics = myItemBeheviourComponentManager->CreateAndRegisterComponent();
		physics->SetToNoSpeed();
		banana->AddComponent(physics);

		CHazardComponent* hazardous = new CHazardComponent;
		CComponentManager::GetInstance().RegisterComponent(hazardous);
		banana->AddComponent(hazardous);

		//adds collider
		SBoxColliderData crystalMeshColliderData;
		crystalMeshColliderData.IsTrigger = true;
		crystalMeshColliderData.myLayer = Physics::eHazzard;
		crystalMeshColliderData.myCollideAgainst = Physics::GetCollideAgainst(crystalMeshColliderData.myLayer);
		crystalMeshColliderData.material.aDynamicFriction = 0.5f;
		crystalMeshColliderData.material.aRestitution = 0.5f;
		crystalMeshColliderData.material.aStaticFriction = 0.5f;
		crystalMeshColliderData.center.y = 0.5f;
		crystalMeshColliderData.myHalfExtent = CU::Vector3f(0.25f, 0.25f, 0.25f);
		CColliderComponent* shellColliderComponent = myColliderManager->CreateComponent(&crystalMeshColliderData, banana->GetId());
		//CGameObject* colliderObject = myGameObjectManager->CreateGameObject();
		CU::Vector3f offset = banana->GetWorldPosition();

		SRigidBodyData rigidbodah;
		rigidbodah.isKinematic = true;
		rigidbodah.useGravity = false;
		rigidbodah.myLayer = Physics::eHazzard;
		rigidbodah.myCollideAgainst = Physics::GetCollideAgainst(rigidbodah.myLayer);

		CColliderComponent* rigidComponent = myColliderManager->CreateComponent(&rigidbodah, banana->GetId());
		//	colliderObject->SetWorldPosition({ offset.x, offset.y + 0.1f, offset.z });
		banana->AddComponent(shellColliderComponent);
		banana->AddComponent(rigidComponent);

		//shell->AddComponent(colliderObject);
		//collider added

		myBananas.Add(banana);
	}
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
		model->FlipVisibility();
		shell->AddComponent(model);

		CParticleEmitterComponent* particle = CParticleEmitterComponentManager::GetInstance().CreateComponent("DriftDebris");
		particle->Deactivate();
		shell->AddComponent(particle);

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

void CItemFactory::CreateRedShellBuffer()
{
	for (int i = 0; i < 25; i++)
	{
		CGameObject* shell = myGameObjectManager->CreateGameObject();
		std::string name = "red shell ";
		name += std::to_string(i);
		shell->SetName(name.c_str());

		CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Shell_Red_01.fbx");
		model->FlipVisibility();
		shell->AddComponent(model);

		CParticleEmitterComponent* particle = CParticleEmitterComponentManager::GetInstance().CreateComponent("DriftDebris");
		particle->Deactivate();
		shell->AddComponent(particle);

		CRedShellBehaviourComponent* behaviour = myRedShellManager->CreateAndRegisterComponent();
		shell->AddComponent(behaviour);


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

		myRedShells.Add(shell);
	}
}

eItemTypes CItemFactory::RandomizeItem()
{
	return static_cast<eItemTypes>(rand() % 5);
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
		shell->NotifyOnlyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		shell->NotifyOnlyComponents(eComponentMessageType::eActivateEmitter, SComponentMessageData());
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
	{
		if (myRedShells.Size() <= 0)
		{
			myRedShells.Add(myActiveRedShells.GetFirst());
			myActiveRedShells.Remove(myActiveRedShells.GetFirst());
		}

		CGameObject* shell = myRedShells.GetLast();
		myRedShells.Remove(shell);
		shell->NotifyOnlyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		shell->NotifyOnlyComponents(eComponentMessageType::eActivateEmitter, SComponentMessageData());
		myActiveRedShells.Add(shell);
		CU::Matrix44f transform = userComponent->GetParent()->GetToWorldTransform();
		CU::Vector3f position = userComponent->GetParent()->GetWorldPosition();
		shell->GetLocalTransform() = transform;
		shell->SetWorldPosition(position);
		//CU::Vector3f forward = userComponent->GetParent()->GetToWorldTransform().myForwardVector;
		//forward  *=3;
		shell->Move(CU::Vector3f(0, -0.5f, 3));
		break;
	}
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
	{
		SComponentMessageData boostData;
		boostData.myBoostData = &myStartBoostData;
		userComponent->GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostData);
		userComponent->GetParent()->NotifyComponents(eComponentMessageType::eMakeInvurnable,boostData);
		break;
	}
	case eItemTypes::eBulletBill:
		//activate AI controller on player and make invincible and boost
		break;
	case eItemTypes::eLightning:
		//postmaster blast all
		break;
	case eItemTypes::eBanana:
	{
		if (myBananas.Size() <= 0)
		{
			myBananas.Add(myActiveBananas.GetFirst());
			myActiveBananas.Remove(myActiveBananas.GetFirst());
		}

		CGameObject* banana = myBananas.GetLast();
		myBananas.Remove(banana);
		banana->NotifyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		myActiveBananas.Add(banana);
		CU::Matrix44f transform = userComponent->GetParent()->GetToWorldTransform();
		CU::Vector3f position = userComponent->GetParent()->GetWorldPosition();
		banana->GetLocalTransform() = transform;
		banana->SetWorldPosition(position);
		//CU::Vector3f forward = userComponent->GetParent()->GetToWorldTransform().myForwardVector;
		//forward  *=3;
		banana->Move(CU::Vector3f(0, 0.f, -2));
		break;
	}
	default:
		break;
	}

	return 0;
}
