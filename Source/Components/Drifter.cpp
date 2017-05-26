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

	return true;
}

void CDrifter::Update(const float aDeltaTime)
{
	myDriftTimer += aDeltaTime;
}

void CDrifter::StartDrifting(const eCurrentAction& aCurrentAction)
{
	switch (aCurrentAction)
	{
	case eCurrentAction::eTurningLeft:
		myIsDrifting = true;
		myDriftRate = myMaxDriftRate;
		myDriftTimer = 0.0f;
		break;
	case eCurrentAction::eTurningRight:
		myIsDrifting = true;
		myDriftRate = -myMaxDriftRate;
		myDriftTimer = 0.0f;
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
