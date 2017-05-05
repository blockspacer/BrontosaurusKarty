#include "stdafx.h"
#include "KartControllerComponent.h"
#include "BoostData.h"

CKartControllerComponent::CKartControllerComponent()
{
	myFowrardSpeed = 0.0f;
	myMaxSpeed = 10.0f;
	myMinSpeed = -5.0f;
	myAcceleration = 0.0f;

	myMaxAcceleration = 100.f;
	myMinAcceleration = -100.f;

	myTurnRate = 1.f;

	myFriction = 10.f;

	mySteering = 0.f;
	myAngularAcceleration = 1.f;

	myMaxSpeedModifier = 1.0f;
	myAccelerationModifier = 1.0f;
}


CKartControllerComponent::~CKartControllerComponent()
{
}

void CKartControllerComponent::TurnRight()
{
	mySteering = myTurnRate;
}

void CKartControllerComponent::TurnLeft()
{
	mySteering = -myTurnRate;
}

void CKartControllerComponent::StopMoving()
{
	myAcceleration = 0;
}

void CKartControllerComponent::MoveFoward()
{
	myAcceleration = myMaxAcceleration;
}

void CKartControllerComponent::MoveBackWards()
{
	myAcceleration = myMinAcceleration;
}

void CKartControllerComponent::StopTurning()
{
	mySteering = 0;
}

void CKartControllerComponent::Update(const float aDeltaTime)
{
	float way = 1.f;
	if (myFowrardSpeed > 0.f)
	{
		way = -1.f;
	}

	myFowrardSpeed += myFriction * way * aDeltaTime;

	myFowrardSpeed += myAcceleration * aDeltaTime * myAccelerationModifier;
	if (myFowrardSpeed > myMaxSpeed * myMaxSpeedModifier)
	{
		myFowrardSpeed = myMaxSpeed + myMaxSpeedModifier;
	}
	if (myFowrardSpeed < myMinSpeed)
	{
		myFowrardSpeed = myMinSpeed;
	}

	float steerAngle = mySteering * myAngularAcceleration * -way;
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	parentTransform.RotateAroundAxis(steerAngle * aDeltaTime, CU::Axees::Y);

	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myFowrardSpeed * aDeltaTime));
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CKartControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eSetBoost:
	{
		myMaxSpeedModifier = 1.0f + aMessageData.myBoostData->maxSpeedBoost;
		myAccelerationModifier = 1.0f + aMessageData.myBoostData->accerationBoost;
	}
	default:
		break;
	}
}