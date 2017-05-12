#include "stdafx.h"
#include "Drifter.h"
#include "..\CommonUtilities\JsonValue.h"


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

	myLongDriftTime = 5.f;
	myShortDriftTime = 2.f;

	return true;
}

void CDrifter::ApplySteering(float& aSteering, const float aDeltaTime)
{
	myDriftTimer += aDeltaTime;
	if (aSteering > 0)
	{
		if (aSteering <= myMaxDriftSteerAffection)
		{
			aSteering += (aDeltaTime / myTimeMultiplier);
		}
	}
	else if (aSteering < 0)
	{
		if (aSteering >= -myMaxDriftSteerAffection)
		{
			aSteering -= (aDeltaTime / myTimeMultiplier);
		}
	}
}

void CDrifter::StartDrifting(const float aSteering)
{
	if (aSteering > 0.f)
	{
		myIsDrifting = true;
		myDriftRate = -myMaxDriftRate;
	}
	else if (aSteering < 0.f)
	{
		myIsDrifting = true;
		myDriftRate = myMaxDriftRate;
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
	myDriftSteerModifier = myDriftSteeringModifier;
}

void CDrifter::TurnLeft()
{
	myDriftSteerModifier = -myDriftSteeringModifier;
}

void CDrifter::StopTurning()
{
	myDriftSteerModifier = 0;
}
