#include "stdafx.h"
#include "DriftTurner.h"
#include "../Physics/PhysXHelper.h"

enum class eLerpCurve : char
{
	eLinear,
	eExponential,
	eEaseOut,
	eEaseIn,
	eSmoothStep,
	eSmootherStep,
};

namespace
{
	float fLerp(float aStart, float aEnd, float aTime)
	{
		return (aStart + aTime*(aEnd - aStart));
		//return (aEnd + aTime*(aStart - aEnd));
	}

	float CalcCurve(float t, eLerpCurve aCurveType)
	{
		const float pi = 3.1415f;

		switch (aCurveType)
		{
		case eLerpCurve::eLinear:
			return t;
		case eLerpCurve::eExponential:
			return t * t;
		case eLerpCurve::eEaseOut:
			return std::sin(t * pi * 0.5f);
		case eLerpCurve::eEaseIn:
			return 1.f - std::cos(t * pi * 0.5f);
		case eLerpCurve::eSmoothStep:
			return t*t * (3.f - 2.f*t);
		case eLerpCurve::eSmootherStep:
			return t*t*t * (t * (6.f*t - 15.f) + 10.f);
		default:
			return 0.0f;
			break;
		}
	}

}
Component::CDriftTurner::CDriftTurner()
{
	myHasBeenHit = false;
	myHasBeenHitTotalTime = 0;
	shouldLerp = false;
}


Component::CDriftTurner::~CDriftTurner()
{
}

void Component::CDriftTurner::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CComponent::Receive(aMessageType, aMessageData);
	switch (aMessageType)
	{
	case eComponentMessageType::eDoDriftBobbing:
	{
		//GetParent()->GetLocalTransform().SetRotation(CU::Matrix33f::CreateRotateAroundY(aMessageData.myFloat));
		//myTargetTurn = CU::Matrix33f::CreateRotateAroundY(aMessageData.myFloat);
		myTargetAngle = aMessageData.myFloat;
		myStartAngle = 0.0f;
		myLerpTimer = 0.0f;
		shouldLerp = true;
		myCurve = eLerpCurve::eEaseIn;

		break;
	}
	case eComponentMessageType::eCancelDriftBobbing:
		GetParent()->GetLocalTransform().SetRotation(CU::Matrix33f::Identity);
		myTargetTurn = CU::Matrix33f::Identity;
		myLerpTimer = 0.0f;
		myCurve = eLerpCurve::eEaseOut;

		myStartAngle = myTargetAngle;
		myTargetAngle = 0.0f;
		shouldLerp = true;
		break;
	case eComponentMessageType::eUpdate:
		DoUpdate(aMessageData.myFloat);
		break;
	case eComponentMessageType::eSpinKart:
	{
		myHasBeenHit = true;
		break;
	}
	}
}

bool Component::CDriftTurner::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	return false;
}

void Component::CDriftTurner::Destroy()
{
	delete this;
}

const float sleprVal = 10.f;
const float lerpTimer = 0.075f;
void Component::CDriftTurner::DoUpdate(const float aDeltaTime)
{
	myLerpTimer += aDeltaTime;
	if (myLerpTimer / lerpTimer >= 1.0f)
	{
		shouldLerp = false;
	}
	if (shouldLerp)
	{
		float angle = fLerp(myStartAngle, myTargetAngle, CalcCurve(CLAMP(myLerpTimer / lerpTimer, 0.0f, 1.0f), myCurve));
		GetParent()->GetLocalTransform().SetRotation(CU::Matrix33f::CreateRotateAroundY(angle));
	}

	//GetParent()->GetLocalTransform().SetRotation(Physics::Slerp(GetParent()->GetLocalTransform().GetRotation(), myTargetTurn, ( 1.f / sleprVal) * aFloat).GetRotation());
	if (myHasBeenHit == true)
	{
		GetParent()->GetLocalTransform().RotateAroundAxis((3.14 * 2 / 1.5) * aDeltaTime * 2.f, CU::Axees::Y);
		myHasBeenHitTotalTime += aDeltaTime;
		if (myHasBeenHitTotalTime > 1.5)
		{
			myHasBeenHit = false;
			myHasBeenHitTotalTime = 0;
			GetParent()->GetLocalTransform().SetRotation(CU::Matrix33f::Identity);
		}

	}
}
