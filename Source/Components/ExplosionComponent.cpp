#include "stdafx.h"
#include "ExplosionComponent.h"


namespace
{
#ifndef RAND_FLOAT_RANGE
#define RAND_FLOAT_RANGE(LOW, HIGH) (LOW) + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((HIGH)-(LOW))));
#endif

	enum class eLerpCurve : char
	{
		eLinear,
		eExponential,
		eEaseOut,
		eEaseIn,
		eSmoothStep,
		eSmootherStep,
		eSickAsHellIn,
		eSickAsHellOut,
	};

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
			return t * t * t * t;
		case eLerpCurve::eEaseOut:
			return std::sin(t * pi * 0.5f);
		case eLerpCurve::eEaseIn:
			return 1.f - std::cos(t * pi * 0.5f);
		case eLerpCurve::eSmoothStep:
			return t*t * (3.f - 2.f*t);
		case eLerpCurve::eSmootherStep:
			return t*t*t * (t * (6.f*t - 15.f) + 10.f);
		case eLerpCurve::eSickAsHellIn:
			return pow(t, 0.3f);
		case eLerpCurve::eSickAsHellOut:
			return 1.0f;
		default:
			return 0.0f;
			break;
		}
	}
	CU::Vector4f vectorFlerp(const CU::Vector4f& aStart, const CU::Vector4f& aEnd, float aTime)
	{

		CU::Vector4f ret;

		ret.x = fLerp(aStart.x, aEnd.x, aTime);
		ret.y = fLerp(aStart.y, aEnd.y, aTime);
		ret.z = fLerp(aStart.z, aEnd.z, aTime);
		ret.w = fLerp(aStart.w, aEnd.w, aTime);

		return ret;
	}

	float Distance2(const CU::Vector4f& p1, const CU::Vector4f& p2)
	{
		return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z));
	}
}

CExplosionComponent::CExplosionComponent()
{
	myDuration = 0.75f;
	myScale = 0.0f;
	myElapsedTime = 0;
	myIsActive = false;
}


CExplosionComponent::~CExplosionComponent()
{
}

void CExplosionComponent::Update(const float aDeltaTime)
{
	if (myIsActive == true)
	{
		
		myElapsedTime += aDeltaTime;

		if (myElapsedTime < 0.05)
		{
			SComponentMessageData data;
			data.myBool = false;
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, data);
		}
		else
		{
			SComponentMessageData data;
			data.myBool = true;
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, data);
			GetParent()->SetWorldPosition(CU::Vector3f(myPosition.x, myPosition.y - 10.5f, myPosition.z));
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}

		if (myElapsedTime < myDuration * 0.5f)
		{
			myScale = fLerp(0.0f, 1.0f,CalcCurve(myElapsedTime/ (myDuration * 0.5f),eLerpCurve::eSickAsHellIn));
			GetParent()->GetLocalTransform().SetScale(CU::Vector3f(myScale, myScale, myScale));
		}
		else if (myElapsedTime >= myDuration * 0.5f)
		{
			myScale = fLerp(1.0f, 0.0f, CalcCurve((myElapsedTime - (myDuration * 0.5f) )/ (myDuration * 0.5f), eLerpCurve::eExponential));
			GetParent()->GetLocalTransform().SetScale(CU::Vector3f(myScale, myScale, myScale));
		}

		if (myElapsedTime >= myDuration)
		{
			SComponentMessageData data;
			data.myBool = false;
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, data);

			GetParent()->NotifyOnlyComponents(eComponentMessageType::eTurnOffHazard, SComponentMessageData());
			myIsActive = false;

			CU::Vector3f position = GetParent()->GetLocalTransform().GetPosition();

			GetParent()->GetLocalTransform() = CU::Matrix44f::Identity;

			GetParent()->GetLocalTransform().SetPosition(position);

			return;
		}

		GetParent()->GetLocalTransform().RotateAroundAxis(aDeltaTime * 3.141592 * 3.5f, CU::Axees::Y);

	}
}

void CExplosionComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case(eComponentMessageType::eResetExplosion):
		{
			myElapsedTime = 0;
			myScale = 0.001;
			myIsActive = true;

			GetParent()->SetWorldPosition(aMessageData.myVector3f);
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());

			myPosition = aMessageData.myVector3f;

			myPosition.y += 10.5f;

			SComponentMessageData data;
			data.myBool = true;
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, data);

			GetParent()->NotifyOnlyComponents(eComponentMessageType::eActivate, data);
			break;
		}

	default:
		break;
	}
}
