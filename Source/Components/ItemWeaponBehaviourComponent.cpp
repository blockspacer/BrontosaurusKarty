#include "stdafx.h"
#include "ItemWeaponBehaviourComponent.h"
#include "../Physics/PhysicsScene.h"


CItemWeaponBehaviourComponent::CItemWeaponBehaviourComponent()
{
	myIsActive = false;
	myIsOnGround = false;
	myGrip = 16;
	myPhysicsScene = nullptr;
	myVelocity = CU::Vector3f::Zero;
	myWeight = 1.5f;
}


CItemWeaponBehaviourComponent::~CItemWeaponBehaviourComponent()
{
}

void CItemWeaponBehaviourComponent::Init(Physics::CPhysicsScene * aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

void CItemWeaponBehaviourComponent::Update(const float aDeltaTime)
{
	if (myIsActive == true)
	{
		DoPhysics(aDeltaTime);


		myVelocity.z += 10.0f;

		GetParent()->GetLocalTransform().Move(myVelocity*aDeltaTime);


		SComponentMessageData messageData;
		messageData.myFloat = aDeltaTime;
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, messageData);

	}
}

const float gravity = 9.82f;
const float upDistConst = 0.01f;
const float testLength = 2.f;

void CItemWeaponBehaviourComponent::DoPhysics(const float aDeltaTime)
{
	const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f upMove = CU::Vector3f::UnitY;
	const float upMoveLength = upMove.Length();
	const float upDist = upDistConst + upMoveLength;
	const float onGroundDist = upDistConst * 2.f + upMoveLength;
	const CU::Vector3f pos = transformation.GetPosition();

	myIsOnGround = false;

	//Update fall speed per wheel

	CU::Vector3f examineVector = pos;

	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector + upMove, down, testLength, Physics::eGround);

	CU::Vector3f downAccl = down;
	float slopeModifier = 1.f;
	float friction = 1.f;
	if (raycastHitData.hit == true)
	{
		const CU::Vector3f& norm = raycastHitData.normal;
		if (raycastHitData.distance < onGroundDist)
		{

			downAccl = norm.Cross(down.Cross(norm));
			friction = norm.Dot(-down);
			myIsOnGround = true;
		}
		if (raycastHitData.distance < upDist)
		{
			const float speed = myVelocity.Length();
			//myVelocity.y = 0.f; downAccl * speed;

			const float disp = upDist - raycastHitData.distance;

			GetParent()->GetLocalTransform().Move(norm * (disp < 0.f ? 0.f : disp));
		}
	}


	myVelocity += downAccl * (friction / (myIsOnGround == true ? myGrip / myWeight : 1.f)) *gravity * aDeltaTime;
}

void CItemWeaponBehaviourComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{

	case eComponentMessageType::eDeactivate:
	{
		myIsActive = false;
	}
	case eComponentMessageType::eActivate:
	{
		myIsActive = true;
	}

	}
}
