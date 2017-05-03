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

	myFriction = 5.f;

	mySteering = 0.f;
	myAngularAcceleration = 1.f;
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
	case eComponentMessageType::eTurnKart:
		mySteering += aMessageData.myFloat;
		break;
	}

	if (mySteering > 1.f)
	{
		mySteering = 1.f;
	}
	else if (mySteering < -1.f)
	{
		mySteering = -1.f;
	}
}

void CKartComponent::Update(float aDeltaTime)
{
	float way = 1.f;
	if (myFowrardSpeed > 0.f)
	{
		way = -1.f;
	}

	myFowrardSpeed += myFriction * way * aDeltaTime;

	myFowrardSpeed += myAcceleration * aDeltaTime;
	if (myFowrardSpeed > myMaxSpeed)
	{
		myFowrardSpeed = myMaxSpeed;
	}
	if (myFowrardSpeed < myMinSpeed)
	{
		myFowrardSpeed = myMinSpeed;
	}
	
	float steerAngle = mySteering * myAngularAcceleration;
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	parentTransform.RotateAroundAxis(steerAngle * aDeltaTime, CU::Axees::Y);

	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myFowrardSpeed * aDeltaTime));
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}
