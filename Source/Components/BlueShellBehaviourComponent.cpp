#include "stdafx.h"
#include "BlueShellBehaviourComponent.h"
#include "LapTrackerComponentManager.h"
#include "../ThreadedPostmaster/BlueShellWarningMessage.h"
#include "GameObjectManager.h"
#include "ModelComponent.h"
#include "ModelComponentManager.h"
#include "HazardComponent.h"
#include "ExplosionComponent.h"
#include "ExplosionComponentManager.h"
#include "ColliderComponentManager.h"
#include "ColliderComponent.h"


CBlueShellBehaviourComponent::CBlueShellBehaviourComponent(CU::GrowingArray<CGameObject*>& aListOfKartObjects, CGameObjectManager* aGameObjectManager, CExplosionComponentManager* aExplosionManager, CColliderComponentManager* aColliderManager)
{
	myKartObjects = &aListOfKartObjects;

	myIsActive = false;
	myTeleportDelay = 2.0f;
	myElapsedTime = 0;
	mySpeed = 30;
	myDecendSpeed = 5;
	myVelocity = CU::Vector3f::UnitZ * mySpeed;
	myAboveHeight = 10;
	myDropSpeed = CU::Vector3f::UnitY * myAboveHeight;


	//create explosion object
	myExplosion = aGameObjectManager->CreateGameObject();

	CModelComponent* model = CModelComponentManager::GetInstance().CreateComponent("Models/Meshes/M_ExplosionSphere_01.fbx");
	model->FlipVisibility();
	myExplosion->AddComponent(model);

	CHazardComponent* hazard = new CHazardComponent;
	hazard->SetToPermanent();
	CComponentManager::GetInstance().RegisterComponent(hazard);
	myExplosion->AddComponent(hazard);

	//explosionComponent
	CExplosionComponent* explosion = aExplosionManager->CreateExplosionComponent();
	myExplosion->AddComponent(explosion);

	//adds collider
	SSphereColliderData crystalMeshColliderData;
	crystalMeshColliderData.IsTrigger = true;
	crystalMeshColliderData.myLayer = Physics::eHazzard;
	crystalMeshColliderData.myCollideAgainst = Physics::GetCollideAgainst(crystalMeshColliderData.myLayer);
	crystalMeshColliderData.material.aDynamicFriction = 0.5f;
	crystalMeshColliderData.material.aRestitution = 0.5f;
	crystalMeshColliderData.material.aStaticFriction = 0.5f;
	crystalMeshColliderData.center.y = 0.0f;
	crystalMeshColliderData.myRadius = 10.5f;
	//crystalMeshColliderData.myHalfExtent = CU::Vector3f(10.5f, 10.5f, 10.5f);
	CColliderComponent* shellColliderComponent = aColliderManager->CreateComponent(&crystalMeshColliderData, myExplosion->GetId());
	//CGameObject* colliderObject = myGameObjectManager->CreateGameObject();

	SRigidBodyData rigidbodah;
	rigidbodah.isKinematic = true;
	rigidbodah.useGravity = false;
	rigidbodah.myLayer = Physics::eHazzard;
	rigidbodah.myCollideAgainst = Physics::GetCollideAgainst(rigidbodah.myLayer);
	CColliderComponent* rigidComponent = aColliderManager->CreateComponent(&rigidbodah, myExplosion->GetId());

	myExplosion->AddComponent(shellColliderComponent);
	myExplosion->AddComponent(rigidComponent);
}


CBlueShellBehaviourComponent::~CBlueShellBehaviourComponent()
{
}

void CBlueShellBehaviourComponent::Update(const float aDeltaTime)
{
	CU::Matrix44f target;

	if (myIsActive == false)
	{
		return;
	}

	if (myElapsedTime < myTeleportDelay)
	{
		myElapsedTime += aDeltaTime;
		CU::Vector3f flyUp(0, myAboveHeight, 0);
		GetParent()->SetWorldPosition(CU::Vector3f(myUser->GetWorldPosition().x,GetParent()->GetWorldPosition().y,myUser->GetWorldPosition().z));
		CU::Vector3f movement = flyUp * aDeltaTime;
		GetParent()->Move(movement);

		if (myElapsedTime >= myTeleportDelay)
		{
			for (int i = 0; i < myKartObjects->Size(); i++)
			{
				char placement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myKartObjects->At(i));
				if (placement == 1)
				{
					target = myKartObjects->At(i)->GetToWorldTransform();
					break;
				}
			}

			target.Move(flyUp);
			GetParent()->GetToWorldTransform().SetPosition(target.GetPosition());
		}

		return;
	}




	for (int i = 0; i < myKartObjects->Size(); i++)
	{
		char placement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myKartObjects->At(i));
		if (placement == 1)
		{
			target = myKartObjects->At(i)->GetToWorldTransform();
			CBlueShellWarningMessage* blue = new CBlueShellWarningMessage(myKartObjects->At(i));

			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(blue);
			break;
		}
	}

	myAboveHeight-=(myDecendSpeed*aDeltaTime);
	myDropSpeed = CU::Vector3f::UnitY * myAboveHeight;

	CU::Matrix44f transform = target;

	//transform.LookAt(target.GetPosition());

	//transform.Move(myVelocity*aDeltaTime);

	transform.Move(myDropSpeed);

	GetParent()->SetWorldTransformation(transform);

	SComponentMessageData data; data.myFloat = aDeltaTime;
	GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, data);

	

}

void CBlueShellBehaviourComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eHazzardCollide:
	{
		myExplosion->SetWorldPosition(aMessageData.myComponent->GetParent()->GetWorldPosition());
		myExplosion->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		SComponentMessageData data;
		data.myVector3f = aMessageData.myComponent->GetParent()->GetWorldPosition();
		myExplosion->NotifyOnlyComponents(eComponentMessageType::eResetExplosion, data);
	}
	case (eComponentMessageType::eDeactivate):
		myIsActive = false;
		break;
	case(eComponentMessageType::eReInitBlueShell):
	{
		myIsActive = true;
		myElapsedTime = 0;
		myAboveHeight = 10;
		myUser = aMessageData.myComponent->GetParent();
		break;
	}

	}
}
