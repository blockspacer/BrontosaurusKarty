#include "stdafx.h"
#include "CameraTilter.h"
#include "../Physics/PhysicsScene.h"


CCameraTilter::CCameraTilter(Physics::CPhysicsScene* aPhysicsScene) : myPhysicsScene(aPhysicsScene), myTargetRotation(0.f), myCurrentRotation(0)
{
}


CCameraTilter::~CCameraTilter()
{
}

void CCameraTilter::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CComponent::Receive(aMessageType, aMessageData);

	switch(aMessageType)
	{
	case eComponentMessageType::eUpdate:
		Update(aMessageData.myFloat);
		break;
	}
}

void CCameraTilter::Destroy()
{
	delete this;
}

void CCameraTilter::Update(const float aDt)
{
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f orig = GetParent()->GetWorldPosition();
	const CU::Vector3f flatForward = GetParent()->GetParent()->GetToWorldTransform().myForwardVector.Normalize();
	const CU::Vector3f forward = GetParent()->GetToWorldTransform().myForwardVector.Normalize();
	Physics::SRaycastHitData firstHitData = myPhysicsScene->Raycast(orig, down, 100.0f, Physics::eGround);
	Physics::SRaycastHitData secondHitData = myPhysicsScene->Raycast(orig + forward * 2.f, down, 100.0f, Physics::eGround);

	myTargetRotation = 0.f;

	if(firstHitData.hit == true && secondHitData.hit == true)
	{
		const CU::Vector3f toSecond = (secondHitData.position - firstHitData.position).Normalize();
		const float dotBetween = CLAMP(toSecond.Dot(flatForward), -1.f, 1.f);
		const bool up = (toSecond.Dot(down) < 0.f ? true : false);

		myTargetRotation = (up ? 1.f : -1.f) * acos(dotBetween);

		

	}

	static const float adaptionSpeed = 5.f;
	myCurrentRotation += ( myTargetRotation - myCurrentRotation) * adaptionSpeed * aDt;

	//GetParent()->GetLocalTransform().SetRotation(CU::Matrix33f::CreateRotateAroundX(myCurrentRotation));
}
