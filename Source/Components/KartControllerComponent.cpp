#include "stdafx.h"
#include "KartControllerComponent.h"
#include "BoostData.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"
#include "../CommonUtilities/JsonValue.h"

CKartControllerComponent::CKartControllerComponent()
{

	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/KartStats.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("Karts");

	CU::CJsonValue Karts = levelsArray.at("BaseKart");



	myFowrardSpeed = 0.0f;
	myMaxSpeed = Karts.at("MaxSpeed").GetFloat();
	myMinSpeed = Karts.at("ReverseSpeed").GetFloat();
	myAcceleration = 0.0f;

	myMaxAcceleration = Karts.at("MaxAcceleration").GetFloat();
	myMinAcceleration = Karts.at("ReverseAcceleration").GetFloat();

	myTurnRate = Karts.at("TurnRate").GetFloat();

	myFriction = 10.f;

	mySteering = 0.f;
	myAngularAcceleration = Karts.at("AngulareAcceleration").GetFloat();

	myMaxSpeedModifier = 1.0f;
	myAccelerationModifier = 1.0f;

	myIsDrifting = false;
	myDriftRate = 0;
	myDriftTimer = 0;
	myDriftSteerModifier = 0;
	myDriftSteeringModifier = 1.3f;
	myMaxDriftRate = 5.5f;
	myTimeMultiplier = 2.5f;
	myMaxDriftSteerAffection = 1.7f;
	myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;

	myLeftWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
	myRightWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
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
		myDriftSteerModifier = myDriftSteeringModifier;
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
		myDriftSteerModifier = -myDriftSteeringModifier;
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
		myDriftRate = -myMaxDriftRate;
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
	}
	else if (mySteering < 0)
	{
		myIsDrifting = true;
		myDriftRate = myMaxDriftRate;
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
	}
}

void CKartControllerComponent::StopDrifting()
{
	CParticleEmitterManager::GetInstance().Deactivate(myLeftWheelDriftEmmiterHandle);
	CParticleEmitterManager::GetInstance().Deactivate(myRightWheelDriftEmmiterHandle);

	if (myDriftTimer >= 2.0f)
	{
		if (myDriftTimer >= 5.0f)
		{
			SComponentMessageData boostMessageData;
			SBoostData* boostData = new SBoostData();
			boostData->accerationBoost = 5.5f;
			boostData->duration = 2.0f;
			boostData->maxSpeedBoost = 1.5f;
			boostData->type = eBoostType::eDefault;
			boostMessageData.myBoostData = boostData;
			GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		}
		else
		{
			SComponentMessageData boostMessageData;
			SBoostData* boostData = new SBoostData();
			boostData->accerationBoost = 5;
			boostData->duration = 1.5f;
			boostData->maxSpeedBoost = 1.0f;
			boostData->type = eBoostType::eDefault;
			boostMessageData.myBoostData = boostData;
			GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		}
	}

	myIsDrifting = false;
	myDriftRate = 0;
	mySteering = 0;
	myDriftTimer = 0;
	myDriftSteerModifier = 0;
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
		myFowrardSpeed -= myBoostSpeedDecay * aDeltaTime;
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
			if (mySteering <= myMaxDriftSteerAffection)
			{
				mySteering += (aDeltaTime / myTimeMultiplier);
			}
		}
		else if (mySteering < 0)
		{
			if (mySteering >= -myMaxDriftSteerAffection)
			{
				mySteering -= (aDeltaTime / myTimeMultiplier);
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
		myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;
	}
	default:
		break;
	}

}