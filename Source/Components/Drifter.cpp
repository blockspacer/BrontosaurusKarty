#include "stdafx.h"
#include "Drifter.h"
#include "..\CommonUtilities\JsonValue.h"
#include "CurrentAction.h"


CDrifter::CDrifter()
{
}

CDrifter::~CDrifter()
{
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
