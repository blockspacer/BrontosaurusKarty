#include "stdafx.h"
#include "ItemFactory.h"
#include "Components/SpeedHandlerManager.h"
#include "../Physics/PhysicsScene.h"
#include "GameObjectManager.h"
#include "ItemWeaponBehaviourComponentManager.h"
#include "ParticleEmitterComponentManager.h"
#include "RedShellManager.h"
#include "BlueShellComponentManager.h"
#include "ColliderComponentManager.h"
#include "../Audio/AudioInterface.h"

#include "ItemWeaponBehaviourComponent.h"
#include "RedShellBehaviourComponent.h"
#include "BlueShellBehaviourComponent.h"
#include "HazardComponent.h"
#include "ModelComponent.h"
#include "ParticleEmitterComponent.h"
#include "PointLightComponent.h"
#include "LightManager.h"

#include "ModelComponentManager.h"
#include "ConcaveMeshCollider.h"

#include "LapTrackerComponentManager.h"

#include "CommonUtilities/JsonValue.h"


CItemFactory::CItemFactory()
{
	myActiveShells.Init(25);
	myShells.Init(25);

	myBananas.Init(50);
	myActiveBananas.Init(50);

	myRedShells.Init(26);
	myActiveRedShells.Init(26);

	myBlueShells.Init(8);
	myActiveBlueShells.Init(8);

	myPlacementDrops.Init(8);
	myLightningBoostBuffer.Init(8);

	CU::CJsonValue boostList;
	std::string filePath = "Json/Items.json";
	const std::string& errorString = boostList.Parse(filePath);
	CU::CJsonValue levelsArray = boostList.at("Items");
	CU::CJsonValue Item = levelsArray.at("Star");

	myStartBoostData.duration = Item.at("Duration").GetFloat();
	myStartBoostData.accerationBoost = Item.at("AccelerationPercentModifier").GetFloat()/100.0f;
	myStartBoostData.maxSpeedBoost = Item.at("MaxSpeedPercentModifier").GetFloat()/100.0f;

	Item = levelsArray.at("Lightning");

	myLightningBoostData.accerationBoost = Item.at("AccelerationPercentModifier").GetFloat() / 100.0f;
	myLightningBoostData.maxSpeedBoost = Item.at("MaxSpeedPercentModifier").GetFloat() / 100.0f;
	myLightningTimeModifier = Item.at("DurationModifier").GetFloat();
}


CItemFactory::~CItemFactory()
{
}

void CItemFactory::Init(CGameObjectManager& aGameObjectManager, CItemWeaponBehaviourComponentManager & aManager, Physics::CPhysicsScene* aPhysicsScene, CColliderComponentManager& aColliderManager, CRedShellManager& aRedShellManager, CBlueShellComponentManager& aBlueShellManager)
{
	myItemBeheviourComponentManager = &aManager;
	myPhysicsScene = aPhysicsScene;
	myGameObjectManager = &aGameObjectManager;
	myColliderManager = &aColliderManager;
	myRedShellManager = &aRedShellManager;
	myBlueShellManager = &aBlueShellManager;

	CreateShellBuffer();
	CreateBananaBuffer();
	CreateRedShellBuffer();
	CreateBlueShellBuffer();
	CreatePlacementDrops();
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

		CParticleEmitterComponent* particle = CParticleEmitterComponentManager::GetInstance().CreateComponent("ShellTrail");
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

void CItemFactory::CreateBlueShellBuffer()
{
	for (int i = 0; i < 5; i++)
	{
		CGameObject* shell = myGameObjectManager->CreateGameObject();
		std::string name = "blue shell ";
		name += std::to_string(i);
		shell->SetName(name.c_str());

		CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_Shell_Blue_01.fbx");
		model->FlipVisibility();
		shell->AddComponent(model);

		CBlueShellBehaviourComponent* behaviour = myBlueShellManager->CreateAndRegisterComponent();
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

		myBlueShells.Add(shell);
	}
}

void CItemFactory::CreatePlacementDrops()
{
	SItemDrop item;
	CU::GrowingArray<SItemDrop> drops; drops.Init(8);

	
	CU::CJsonValue itemDropList;
	std::string filePath = "Json/ItemDrops.json";
	const std::string& errorString = itemDropList.Parse(filePath);

	for (int i = 1; i < itemDropList.Size()+1; i++)
	{
		std::stringstream temp1;
		temp1 << i;
		std::string temp2 = temp1.str();
		CU::CJsonValue levelsArray = itemDropList.at(temp2);
		for (int i = 0; i < levelsArray.Size(); i++)
		{
			CU::CJsonValue jsonItem = levelsArray[i];
			item.myChance = jsonItem.at("DropRate").GetUchar();
			item.myType = CheckItem(jsonItem.at("ItemType").GetString());
			drops.Add(item);
		}
		myPlacementDrops.Add(drops);
		drops.RemoveAll();
	}


}

eItemTypes CItemFactory::CheckItem(std::string aItem)
{
	eItemTypes item = eItemTypes::eBanana;
	if (aItem == "Banana")
	{
		item = eItemTypes::eBanana;
	}
	if (aItem == "GreenShell")
	{
		item = eItemTypes::eGreenShell;
	}
	if (aItem == "RedShell")
	{
		item = eItemTypes::eRedShell;
	}
	if (aItem == "BlueShell")
	{
		item = eItemTypes::eBlueShell;
	}
	if (aItem == "Mushroom")
	{
		item = eItemTypes::eMushroom;
	}
	if (aItem == "Star")
	{
		item = eItemTypes::eStar;
	}
	if (aItem == "Lightning")
	{
		item = eItemTypes::eLightning;
	}

	return item;
}

eItemTypes CItemFactory::RandomizeItem(CComponent* aPlayerCollider)
{
	unsigned char placement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(aPlayerCollider->GetParent()) - 1;
	char itemrange = 1;

	char result = 0;
	eItemTypes item = eItemTypes::eBanana;

	result = (rand() % 100)+1;

	for (int i = 0; i < myPlacementDrops[placement].Size(); i++)
	{
		if (result <= myPlacementDrops[placement][i].myChance)
		{
			 item = myPlacementDrops[placement][i].myType;
			 break;
		}
	}

	return item;
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
		SComponentMessageData data; data.myComponent = userComponent;
		shell->NotifyOnlyComponents(eComponentMessageType::eReInitRedShell, data);
		break;
	}
	case eItemTypes::eBlueShell:
	{
		if (myBlueShells.Size() <= 0)
		{
			myBlueShells.Add(myActiveBlueShells.GetFirst());
			myActiveBlueShells.Remove(myActiveBlueShells.GetFirst());
		}

		CGameObject* shell = myBlueShells.GetLast();
		myBlueShells.Remove(shell);
		shell->NotifyOnlyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		myActiveBlueShells.Add(shell);
		CU::Matrix44f transform = userComponent->GetParent()->GetToWorldTransform();
		CU::Vector3f position = userComponent->GetParent()->GetWorldPosition();
		shell->GetLocalTransform() = transform;
		shell->SetWorldPosition(position);
		//CU::Vector3f forward = userComponent->GetParent()->GetToWorldTransform().myForwardVector;
		//forward  *=3;
		shell->Move(CU::Vector3f(0, 0.5f, 3));

		SComponentMessageData data;
		data.myComponent = userComponent;
		shell->NotifyOnlyComponents(eComponentMessageType::eReInitBlueShell, data);
		break;
	}
	case eItemTypes::eMushroom:
	{
		SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPowerUp"));
		userComponent->GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		//userComponent->GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost,)
		break;
	}
	case eItemTypes::eStar:
	{
		SComponentMessageData boostData;
		boostData.myBoostData = &myStartBoostData;
		userComponent->GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostData);
		userComponent->GetParent()->NotifyComponents(eComponentMessageType::eMakeInvurnable,boostData);
		break;
	}
	case eItemTypes::eLightning:
	{
		for (int i = 0; i < myRedShellManager->GetKarts().Size(); i++)
		{
			unsigned char placement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myRedShellManager->GetKarts()[i]);
			unsigned char userplacement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(userComponent->GetParent());
			if (myRedShellManager->GetKarts()[i] != userComponent->GetParent() && userplacement > placement)
			{
				myRedShellManager->GetKarts()[i]->NotifyOnlyComponents(eComponentMessageType::eGotHit, SComponentMessageData());


				myLightningBoostData.duration = (myRedShellManager->GetKarts().Size() - placement)*myLightningTimeModifier;
				myLightningBoostBuffer.Add(myLightningBoostData);

				SComponentMessageData slowdata; 
				slowdata.myBoostData = &myLightningBoostBuffer.GetLast();
				myRedShellManager->GetKarts()[i]->NotifyOnlyComponents(eComponentMessageType::eGiveBoost, slowdata);
			}
		}
		myLightningBoostBuffer.RemoveAll();
		Audio::CAudioInterface::GetInstance()->PostEvent("PlayLightning");
		break;
	}
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
