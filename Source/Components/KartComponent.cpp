#include "stdafx.h"
#include "KartComponent.h"


CKartComponent::CKartComponent()
{
	mySpeed = 0.0f;
	myMaxSpeed = 10.0f;
	myAcceleration = 0.0f;
	myDecceleration = 5.0f;
}


CKartComponent::~CKartComponent()
{
}

void CKartComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData&  aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAccelerate:
		myAcceleration = aMessageData.myFloat;
		break;
	case eComponentMessageType::eStopAcceleration:
		myAcceleration = 0.f;
		break;
	case eComponentMessageType::eDecelerate:
		myAcceleration = aMessageData.myFloat;
		break;
	case eComponentMessageType::eStopDeceleration:
		myAcceleration = 0.f;
		break;
	}
}

void CKartComponent::Update(float aDeltaTime)
{
	DL_PRINT("kart speed %f", mySpeed);

	if (myAcceleration > 0.0f)
	{
		mySpeed += myAcceleration * aDeltaTime;
		if(mySpeed > myMaxSpeed)
		{
			mySpeed = myMaxSpeed;
		}
	}
	else
	{
		mySpeed -= aDeltaTime * myDecceleration;
	}

	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, mySpeed * aDeltaTime));
}
