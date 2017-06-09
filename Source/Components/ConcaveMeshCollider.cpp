#include "stdafx.h"
#include "ConcaveMeshCollider.h"

#include "../Physics/Foundation.h"
#include "../Physics/Physics.h"
#include "../Physics/Shape.h"
#include "../Physics/PhysicsScene.h"

CConcaveMeshColliderComponent::CConcaveMeshColliderComponent(const SConcaveMeshColliderData& aColliderData, Physics::CPhysicsScene* aScene)
	: CColliderComponent(aColliderData, nullptr, nullptr, false)
	, myScene(aScene)
{
	myData = aColliderData;
}

CConcaveMeshColliderComponent::~CConcaveMeshColliderComponent()
{
}

void CConcaveMeshColliderComponent::ReInit(const CU::Vector3f& aScale)
{
	myScene->RemoveActor(myActor);
	SAFE_DELETE(myActor);
	SAFE_DELETE(myShape);
	Init(aScale);
}

void CConcaveMeshColliderComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponent == this && myActor == nullptr)
		{
			const CU::Vector3f scale = GetParent()->GetToWorldTransform().GetScale();
			Init(scale);
		}
	}

	CColliderComponent::Receive(aMessageType, aMessageData);
}

void CConcaveMeshColliderComponent::Init(const CU::Vector3f& aScale)
{
	Physics::CPhysics* physics = Physics::CFoundation::GetInstance()->GetPhysics();
	Physics::SMaterialData material;
	Physics::CShape* shape = physics->CreateConcaveMeshShape(myData.myPath, material, aScale);

	if(myData.myLayer == Physics::eWall)
	{
		const int i = 0;
	}

	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
	}

	shape->SetCollisionLayers(myData.myLayer, myData.myCollideAgainst);
	if (GetParent() != nullptr)
	{
		shape->SetObjectId(GetParent()->GetId());
	}

	Physics::CPhysicsActor* actor = physics->CreateStaticActor(shape, myData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
	}
	myScene->AddActor(actor);
	myActor = actor;
	myShape = shape;
	myActor->SetCallbackData(this);
}
