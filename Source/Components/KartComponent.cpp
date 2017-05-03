#include "stdafx.h"
#include "KartComponent.h"


CKartComponent::CKartComponent()
{
	myFowrardSpeed = 0.0f;
	myMaxSpeed = 10.0f;
	myMinSpeed = -5.0f;
	myAcceleration = 0.0f;

	myMaxAcceleration = 100.f;
	myMinAcceleration = -100.f;

	myFriction = 50.f;
}


CKartComponent::~CKartComponent()
{
}

void CKartComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData&  aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAccelerate:
		myAcceleration = myMaxAcceleration;
		break;
	case eComponentMessageType::eStopAcceleration:
		myAcceleration = 0.f;
		break;
	case eComponentMessageType::eDecelerate:
		myAcceleration = myMinAcceleration;
		break;
	case eComponentMessageType::eStopDeceleration:
		myAcceleration = 0.f;
		break;
	}
}

void CKartComponent::Update(float aDeltaTime)
{
	float way = 1.f;
	if (myAcceleration > 0.f)
	{
		way = -1.f;
	}

	myAcceleration += myFriction * way * aDeltaTime;

	myFowrardSpeed += myAcceleration * aDeltaTime;
	if (myFowrardSpeed > myMaxSpeed)
	{
		myFowrardSpeed = myMaxSpeed;
	}
	if (myFowrardSpeed < myMinSpeed)
	{
		myFowrardSpeed = myMinSpeed;
	}

	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myFowrardSpeed * aDeltaTime));
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}
