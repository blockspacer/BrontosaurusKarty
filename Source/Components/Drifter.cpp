#include "stdafx.h"
#include "Drifter.h"
#include "..\CommonUtilities\JsonValue.h"
#include "CurrentAction.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"


CDrifter::CDrifter() : myLargeBoostOn(false), mySmallBoostOn(false)
{
}

CDrifter::~CDrifter()
{
	CParticleEmitterManager::GetInstance().Release(myLeftDriftDustEmitterHandle);
	CParticleEmitterManager::GetInstance().Release(myRightDriftDustEmitterHandle);
	CParticleEmitterManager::GetInstance().Release(myLeftSmallBoostReadyEmitterHandle);
	CParticleEmitterManager::GetInstance().Release(myRightSmallBoostReadyEmitterHandle);
	CParticleEmitterManager::GetInstance().Release(myLeftLargeBoostReadyEmitterHandle);
	CParticleEmitterManager::GetInstance().Release(myRightLargeBoostReadyEmitterHandle);

}

bool CDrifter::Init(const CU::CJsonValue& aJsonValue)
{
	myIsDrifting = false;
	myDriftRate = 0;
	myDriftTimer = 0;
	myDriftSteerModifier = 0;
	myDriftSteeringModifier = aJsonValue.at("DriftTurnRate").GetFloat();
	myMaxDriftRate = aJsonValue.at("MaxDriftRate").GetFloat();
	myTimeMultiplier = aJsonValue.at("DriftOverTimeMultiplier").GetFloat();
	myMaxDriftSteerAffection = aJsonValue.at("MaxDriftTurnRate").GetFloat();

	myLongDriftTime = aJsonValue.at("LargeBoostAt").GetFloat();
	myShortDriftTime = aJsonValue.at("SmallBoostAt").GetFloat();

	mySlowExtraSpeed = aJsonValue.at("SmallDriftSpeed").GetFloat();
	myFastExtraSpeed = aJsonValue.at("LargeDriftSpeed").GetFloat();

	myLeftSmallBoostReadyEmitterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(aJsonValue.at("SmallBoostParticle").GetString());
	myRightSmallBoostReadyEmitterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(aJsonValue.at("SmallBoostParticle").GetString());
	myLeftDriftDustEmitterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(aJsonValue.at("DriftParticle").GetString());
	myRightDriftDustEmitterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(aJsonValue.at("DriftParticle").GetString());
	myLeftLargeBoostReadyEmitterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(aJsonValue.at("LargeBoostParticle").GetString());
	myRightLargeBoostReadyEmitterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(aJsonValue.at("LargeBoostParticle").GetString());

	myDriftState = eDriftState::eNotDrifting;

	return true;
}

void CDrifter::GetSteering(float& aSteeringAngle, const float aTurnRate, const float aAngularAcceleration, const float aWay,const float aAirControl, const bool aIsOnGround, const eCurrentAction& aAction ,const float aDeltaTime)
{
	myDriftTimer += aDeltaTime;
	switch (myDriftState)
	{
	case eDriftState::eDriftingLeft:
			aSteeringAngle = (-aTurnRate + GetSteerModifier()) * aAngularAcceleration * -aWay * (aIsOnGround == true ? 1.f : aAirControl);
		break;
	case eDriftState::eDriftingRight:
			aSteeringAngle = (aTurnRate + GetSteerModifier()) * aAngularAcceleration * -aWay * (aIsOnGround == true ? 1.f : aAirControl);
		break;
	default:
		break;
	}
}

void CDrifter::StartDrifting(const eCurrentAction& aCurrentAction)
{
	switch (aCurrentAction)
	{
	case eCurrentAction::eTurningLeft:
		myIsDrifting = true;
		myDriftRate = myMaxDriftRate;
		myDriftTimer = 0.0f;
		myDriftState = eDriftState::eDriftingLeft;
		break;
	case eCurrentAction::eTurningRight:
		myIsDrifting = true;
		myDriftRate = -myMaxDriftRate;
		myDriftTimer = 0.0f;
		myDriftState = eDriftState::eDriftingRight;
		break;
	default:
		break;
	}
	SetDriftParticlesReady(true);
}

CDrifter::eDriftBoost CDrifter::StopDrifting()
{
	eDriftBoost boost = eDriftBoost::eNone;

	if (myDriftTimer >= myLongDriftTime)
	{
		boost = eDriftBoost::eLarge;
	}
	else if (myDriftTimer >= myShortDriftTime)
	{
		boost = eDriftBoost::eSmall;
	}

	myIsDrifting = false;
	myDriftRate = 0;
	myDriftTimer = 0;
	myDriftSteerModifier = 0;
	myDriftState = eDriftState::eNotDrifting;
	SetDriftParticlesReady(false);
	SetSmallBoostReady(false);
	SetLargeBoostReady(false);
	return boost;
}

void CDrifter::TurnRight()
{
	myDriftSteerModifier = std::fabs(myDriftSteeringModifier);
}

void CDrifter::TurnLeft()
{
	myDriftSteerModifier = -myDriftSteeringModifier;
}

void CDrifter::StopTurning()
{
	myDriftSteerModifier = 0;
}

void CDrifter::UpdateDriftParticles(const CU::Matrix44f & aKartOrientation)
{
	CU::Matrix44f particlePosition = aKartOrientation;

	particlePosition.Move(CU::Vector3f(-0.45f, 0, 0));
	CParticleEmitterManager::GetInstance().SetPosition(myLeftDriftDustEmitterHandle, particlePosition.GetPosition());
	CParticleEmitterManager::GetInstance().SetPosition(myLeftSmallBoostReadyEmitterHandle, particlePosition.GetPosition());
	CParticleEmitterManager::GetInstance().SetPosition(myLeftLargeBoostReadyEmitterHandle, particlePosition.GetPosition());
	particlePosition.Move(CU::Vector3f(0.9f, 0, 0));
	CParticleEmitterManager::GetInstance().SetPosition(myRightDriftDustEmitterHandle, particlePosition.GetPosition());
	CParticleEmitterManager::GetInstance().SetPosition(myRightSmallBoostReadyEmitterHandle, particlePosition.GetPosition());
	CParticleEmitterManager::GetInstance().SetPosition(myRightLargeBoostReadyEmitterHandle, particlePosition.GetPosition());;

	if (myDriftTimer > myShortDriftTime)
	{
		if (myDriftTimer > myLongDriftTime)
		{
			SetSmallBoostReady(false);
			SetLargeBoostReady(true);
			return;
		}
		SetSmallBoostReady(true);
	}
}

void CDrifter::SetDriftParticlesReady(const bool aFlag)
{
	if (aFlag == true)
	{
		CParticleEmitterManager::GetInstance().Activate(myLeftDriftDustEmitterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightDriftDustEmitterHandle);
		return;
	}
	CParticleEmitterManager::GetInstance().Deactivate(myLeftDriftDustEmitterHandle);
	CParticleEmitterManager::GetInstance().Deactivate(myRightDriftDustEmitterHandle);
}
void CDrifter::SetSmallBoostReady(const bool aFlag)
{
	if (aFlag == true && mySmallBoostOn == false)
	{
		
		CParticleEmitterManager::GetInstance().Activate(myLeftSmallBoostReadyEmitterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightSmallBoostReadyEmitterHandle);
		
	}
	else if(aFlag == false && mySmallBoostOn == true)
	{
		CParticleEmitterManager::GetInstance().Deactivate(myLeftSmallBoostReadyEmitterHandle);
		CParticleEmitterManager::GetInstance().Deactivate(myRightSmallBoostReadyEmitterHandle);
	}
	mySmallBoostOn = aFlag;
}
void CDrifter::SetLargeBoostReady(const bool aFlag)
{
	if (aFlag == true)
	{
		CParticleEmitterManager::GetInstance().Activate(myLeftLargeBoostReadyEmitterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightLargeBoostReadyEmitterHandle);
	}
	else if(aFlag == false && myLargeBoostOn == true)
	{
		CParticleEmitterManager::GetInstance().Deactivate(myLeftLargeBoostReadyEmitterHandle);
		CParticleEmitterManager::GetInstance().Deactivate(myRightLargeBoostReadyEmitterHandle);
	}

	myLargeBoostOn = aFlag;;
}

