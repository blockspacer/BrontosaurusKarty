#include "stdafx.h"
#include "ColliderComponentManager.h"
#include "CharacterControllerComponent.h"
#include "EffectHelper.h"
#include "ConcaveMeshCollider.h"

#include "..\Physics\Physics.h"
#include "..\Physics\PhysicsScene.h"
#include "..\Physics\Shape.h"
#include "..\Physics\PhysicsActor.h"
#include "..\Physics\PhysicsActorDynamic.h"
#include "..\Physics\CharacterControllerManager.h"
#include "..\Physics\PhysicsCharacterController.h"
#include "..\Physics\CollisionLayers.h"
#include "..\Physics\Collection.h"

const std::nullptr_t nopeptr_constant = 0x0;
#define nopeptr nopeptr_constant

CColliderComponentManager::CColliderComponentManager()
{
	myColliderComponents.Init(128);
	myControllerManager = nullptr;
}

CColliderComponentManager::~CColliderComponentManager()
{
	myColliderComponents.RemoveAll();

	if(myControllerManager)
		delete myControllerManager;
	myControllerManager= nullptr;
}

CColliderComponent* CColliderComponentManager::CreateComponent(SColliderData* aColliderData, ComponentId anId)
{
	CColliderComponent* newComponent = nullptr;
	switch (aColliderData->myType)
	{
	case SColliderData::eColliderType::eBox:
		newComponent = (CreateBoxCollider(*reinterpret_cast<SBoxColliderData*>(aColliderData),anId));
		break;
	case SColliderData::eColliderType::eSphere:
		newComponent = (CreateSphereCollider(*reinterpret_cast<SSphereColliderData*>(aColliderData),anId));
		break;
	case SColliderData::eColliderType::eCapsule:
		newComponent = (CreateCapsuleCollider(*reinterpret_cast<SCapsuleColliderData*>(aColliderData),anId));
		break;
	case SColliderData::eColliderType::eMesh:
		{
			newComponent = CreateMeshCollider(*reinterpret_cast<SMeshColliderData*>(aColliderData), anId);
		}
		break;
	case SColliderData::eColliderType::eConcaveMesh:
		newComponent = (CreateConcaveMeshCollider(*static_cast<SConcaveMeshColliderData*>(aColliderData), anId));
		break;
	case SColliderData::eColliderType::eRigidbody:
		newComponent = (CreateRigidbody(*reinterpret_cast<SRigidBodyData*>(aColliderData),anId));
		break;
	default:
		break;
	}
	if (!newComponent)
	{
		return nullptr;
	}

	if (aColliderData->myType != SColliderData::eColliderType::eRigidbody &&
		aColliderData->myType != SColliderData::eColliderType::eConcaveMesh)
	{
		newComponent->GetShape()->SetCollisionLayers(aColliderData->myLayer, aColliderData->myCollideAgainst);
	}

	myColliderComponents.Add(newComponent);
	CComponentManager::GetInstance().RegisterComponent(newComponent);
	return newComponent;
}

void CColliderComponentManager::Update()
{
	for (CColliderComponent* collider : myColliderComponents)
	{
		collider->UpdateCallbacks();
		if (collider->GetType() == SColliderData::eColliderType::eRigidbody)
		{
			collider->UpdatePosition();
		}
	}
}

void CColliderComponentManager::InitControllerManager()
{
	myControllerManager = new Physics::CCharacterControllerManager(myScene, myPhysics);
}

void CColliderComponentManager::RemoveActorFromScene(Physics::CPhysicsActor* aActor)
{
	myScene->RemoveActor(aActor);
}

CCharacterControllerComponent* CColliderComponentManager::CreateCharacterControllerComponent(const Physics::SCharacterControllerDesc& aParams, ComponentId anId)
{
	Physics::CPhysicsCharacterController* controller = myControllerManager->CreateCharacterController(aParams, anId);
	controller->SetParentId(anId);
	CCharacterControllerComponent* component = new CCharacterControllerComponent(controller, aParams.halfHeight, aParams.radius, aParams.center);
	CComponentManager::GetInstance().RegisterComponent(component);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateBoxCollider(const SBoxColliderData& aBoxColliderData, ComponentId anId)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateBoxShape(aBoxColliderData.myHalfExtent, material);
	
	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
		return nullptr;
	}

	shape->SetCollisionLayers(aBoxColliderData.myLayer, aBoxColliderData.myCollideAgainst);
	shape->SetObjectId(anId);

	Physics::CPhysicsActor* actor = nullptr;
		
	
	actor = myPhysics->CreateStaticActor(shape, aBoxColliderData.IsTrigger);
	
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
		return nullptr;
	}
	myScene->AddActor(actor);
	CBoxColliderComponent* component = new CBoxColliderComponent(aBoxColliderData, shape, actor, aBoxColliderData.IsTrigger);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateSphereCollider(const SSphereColliderData& aSphereColliderData, ComponentId anId)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateSphereShape(aSphereColliderData.myRadius, material);

	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
		return nullptr;
	}

	shape->SetCollisionLayers(aSphereColliderData.myLayer, aSphereColliderData.myCollideAgainst);
	shape->SetObjectId(anId);

	Physics::CPhysicsActor* actor = myPhysics->CreateStaticActor(shape, aSphereColliderData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
		return nullptr;
	}
	myScene->AddActor(actor);
	CSphereColliderComponent* component = new CSphereColliderComponent(aSphereColliderData, shape, actor, aSphereColliderData.IsTrigger);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateCapsuleCollider(const SCapsuleColliderData& aCapsuleColliderData, ComponentId anId)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateCapsuleShape(aCapsuleColliderData.myRadius, aCapsuleColliderData.myHeight / 2.0f, material);
	shape->SetObjectId(0);
	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
		return nullptr;
	}

	shape->SetCollisionLayers(aCapsuleColliderData.myLayer, aCapsuleColliderData.myCollideAgainst);
	shape->SetObjectId(anId);
	Physics::CPhysicsActor* actor = myPhysics->CreateStaticActor(shape, aCapsuleColliderData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
		return nullptr;
	}
	myScene->AddActor(actor);
	CCapsuleColliderComponent* component = new CCapsuleColliderComponent(aCapsuleColliderData, shape, actor, aCapsuleColliderData.IsTrigger);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateMeshCollider(const SMeshColliderData& aMeshColliderData, ComponentId anId)
{
	std::string path(aMeshColliderData.myPath);
	if (EffectHelper::FileExists(std::wstring(path.begin(), path.end()).c_str()) == false)
	{
		DL_ASSERT((std::string("Mesh collider \"") + aMeshColliderData.myPath + "\" could not be found.").c_str());
		return nullptr;
	}

	
	CMeshColliderComponent* component = new CMeshColliderComponent(aMeshColliderData, myScene);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateConcaveMeshCollider(const SConcaveMeshColliderData& aMeshColliderData, ComponentId anId)
{
	if (!std::ifstream(aMeshColliderData.myPath).good())
	{
		//DL_MESSAGE_BOX("Mesh collider not found %s", aMeshColliderData.myPath);
		return nopeptr;
	}
	
	return new CConcaveMeshColliderComponent(aMeshColliderData, myScene);
}

CColliderComponent* CColliderComponentManager::CreateRigidbody(const SRigidBodyData& aRigidbodyData, ComponentId anId)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = nullptr;

	Physics::CPhysicsActorDynamic* actor = reinterpret_cast<Physics::CPhysicsActorDynamic*>(myPhysics->CreateDynamicActor(
		shape, 
		aRigidbodyData.IsTrigger,
		aRigidbodyData.mass, 
		aRigidbodyData.isKinematic, 
		aRigidbodyData.useGravity));

	if(shape != nullptr)
	{	
		shape->SetCollisionLayers(aRigidbodyData.myLayer, aRigidbodyData.myCollideAgainst);
		shape->SetObjectId(anId);
	}

	actor->SetRotationLock(aRigidbodyData.freezedRotationAxiees);
	myScene->AddActor(actor);
	CRigidBodyComponent* component = new CRigidBodyComponent(aRigidbodyData, actor, aRigidbodyData.IsTrigger);
	component->SetManager(this);
	return component;
}
