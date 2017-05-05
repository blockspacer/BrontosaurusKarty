#include "stdafx.h"
#include "KartControllerComponent.h"
#include "BoostData.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"

CKartControllerComponent::CKartControllerComponent()
{
	myFowrardSpeed = 0.0f;
	myMaxSpeed = 30.0f;
	myMinSpeed = -10.0f;
	myAcceleration = 0.0f;

	myMaxAcceleration = 100.f;
	myMinAcceleration = -100.f;

	myTurnRate = 1.f;

	myFriction = 10.f;

	mySteering = 0.f;
	myAngularAcceleration = 1.f;

	myMaxSpeedModifier = 1.0f;
	myAccelerationModifier = 1.0f;

	myIsDrifting = false;
	myDriftRate = 0;
	myDriftTimer = 0;
	myDriftSteerModifier = 0;

	myLeftWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("GatlingSmoke");
	myRightWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("GatlingSmoke");
}


CKartControllerComponent::~CKartControllerComponent()
{
}

void CKartControllerComponent::TurnRight()
{
	if (myIsDrifting == false)
	{
		mySteering = myTurnRate;
	}
	else
	{
		myDriftSteerModifier = 1.3f;
	}
}

void CKartControllerComponent::TurnLeft()
{
	if (myIsDrifting == false)
	{
		mySteering = -myTurnRate;
	}
	else
	{
		myDriftSteerModifier = -1.3f;
	}
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
	if (myIsDrifting == false)
	{
		mySteering = 0;
	}
	else
	{
		myDriftSteerModifier = 0;
	}
}

//Checks if the player is turning left or right and then sets the drift values accordingly
void CKartControllerComponent::Drift()
{
	if (mySteering > 0)
	{
		myIsDrifting = true;
		myDriftRate = -5.5f;
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
	}
	else if (mySteering < 0)
	{
		myIsDrifting = true;
		myDriftRate = 5.5f;
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
	}
}

void CKartControllerComponent::StopDrifting()
{
	myIsDrifting = false;
	myDriftRate = 0;
	mySteering = 0;
	myDriftSteerModifier = 0;
	CParticleEmitterManager::GetInstance().Deactivate(myLeftWheelDriftEmmiterHandle);
	CParticleEmitterManager::GetInstance().Deactivate(myRightWheelDriftEmmiterHandle);
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
		myFowrardSpeed = myMaxSpeed * myMaxSpeedModifier;
	}
	if (myFowrardSpeed < myMinSpeed)
	{
		myFowrardSpeed = myMinSpeed;
	}

	float steerAngle = (mySteering + myDriftSteerModifier) * myAngularAcceleration * -way;
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	parentTransform.RotateAroundAxis(steerAngle * aDeltaTime, CU::Axees::Y);

	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myFowrardSpeed * aDeltaTime));
	if (myIsDrifting == true)
	{
		myDriftTimer += aDeltaTime;
		if (mySteering > 0)
		{
			if (mySteering <= 1.7f)
			{
				mySteering += (aDeltaTime / 2.5f);
			}
		}
		else if (mySteering < 0)
		{
			if (mySteering >= -1.7f)
			{
				mySteering -= (aDeltaTime / 2.5f);
			}
		}
		GetParent()->GetLocalTransform().Move(CU::Vector3f(myDriftRate * aDeltaTime, 0.0f, 0.0f));
		CU::Matrix44f particlePosition = GetParent()->GetLocalTransform();
		particlePosition.Move(CU::Vector3f(-0.45f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myLeftWheelDriftEmmiterHandle, particlePosition.GetPosition());
		particlePosition.Move(CU::Vector3f(0.9f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myRightWheelDriftEmmiterHandle, particlePosition.GetPosition());
	}
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