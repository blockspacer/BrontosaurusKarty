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
	{
		myAcceleration = aMessageData.myFloat;
	}
	case eComponentMessageType::eStopAccelerate:
	{
		myAcceleration = 0.0f;
	}
	default:
		break;
	}
}

void CKartComponent::Update(float aDeltaTime)
{
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
}