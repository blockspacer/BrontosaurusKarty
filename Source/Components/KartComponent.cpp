#include "stdafx.h"
#include "KartComponent.h"

#define MIN(A,B) ((A) < (B)) ? A : B
#define MAX(A,B) ((A) > (B)) ? A : B
#define RANGE(A,B,C) ((A) < (B)) ? B : ((A) > (C)) ? C : A
#define CLAMP(V,A_MIN,A_MAX) RANGE(V,A_MIN,A_MAX)

CKartComponent::CKartComponent()
{
	myFowrardSpeed = 0.0f;
	myMaxSpeed = 20.0f;
	myMinSpeed = -5.0f;
	myAcceleration = 0.0f;

	myMaxAcceleration = 100.f;
	myMinAcceleration = -30.f;

	myFriction = 10.f;

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
	case eComponentMessageType::eStopTurnKart:
		mySteering = 0.f;
		break;
	}

	//CLAMP(mySteering, -1.f, 1.f);
	//CLAMP(myAcceleration, myMinAcceleration, myMaxAcceleration);
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

	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();

	if (mySteering != 0.f)
	{
		float steerAngle = mySteering * myAngularAcceleration * -way * aDeltaTime;
		parentTransform.RotateAroundAxis(steerAngle, CU::Axees::Y);
	}

	if (myFowrardSpeed != 0.f)
	{
		parentTransform.Move(CU::Vector3f(0.0f, 0.0f, myFowrardSpeed * aDeltaTime));
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	}
}
