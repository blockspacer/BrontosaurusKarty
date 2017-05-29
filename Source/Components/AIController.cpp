#include "stdafx.h"
#include "AIController.h"
#include "KartControllerComponent.h"
#include "..\Game\NavigationSpline.h"
#include "..\CommonUtilities\line.h"



constexpr float maxAngleInv = 1.0f / (2.f * 3.1415f);



CU::Vector2f pDistance(const float x, const float y, const float x1, const float y1, const float x2, const float y2)
{

	float A = x - x1;
	float B = y - y1;
	float C = x2 - x1;
	float D = y2 - y1;

	float dot = A * C + B * D;
	float len_sq = C * C + D * D;
	float param = -1;
	if (len_sq != 0) //in case of 0 length line
		param = dot / len_sq;

	float xx, yy;

	if (param < 0) {
		xx = x1;
		yy = y1;
	}
	else if (param > 1) {
		xx = x2;
		yy = y2;
	}
	else {
		xx = x1 + param * C;
		yy = y1 + param * D;
	}

	return CU::Vector2f(xx, yy);
}



CAIController::CAIController(CKartControllerComponent& aKartComponent) : CController(aKartComponent)
{
	myCurrentSplineIndex = 0;
	myTimer = 0.f;
	myState = eStates::eStop;
	myIsDrifting = 0;
}


CAIController::~CAIController()
{
}

float signof(float aFloat)
{
	if (aFloat < 0.f)
	{
		return -1.f;
	}
	return 1.f;
}

template<typename T, typename U>
bool samesign(const T aLeft, const U aRight)
{
	if (aLeft > 0)
	{
		if (aRight > 0)
		{
			return true;
		}
		return false;
	}
	if (aRight < 0)
	{
		return true;
	}
}

void CAIController::Update(const float aDeltaTime)
{
	CU::Vector3f piss = myControllerComponent.GetParent()->GetToWorldTransform().GetPosition();
	CU::Vector2f pos(piss.x, piss.z);

	CU::Vector3f forward = myControllerComponent.GetParent()->GetToWorldTransform().myForwardVector;
	CU::Vector2f kartForward(forward.x, forward.z);
	kartForward.Normalize();
	pos += kartForward * 5.f;

	const SNavigationPoint * currentSpline = &GetNavigationSpline().GetPoint(myCurrentSplineIndex);


	if (currentSpline->myForwardDirection.Dot(currentSpline->myPosition - pos) < 0.0f
		&& myControllerComponent.GetIsGrounded())
	{
		myCurrentSplineIndex++;
		myCurrentSplineIndex %= GetNavigationSpline().GetNumberOfPoints();
		currentSpline = &GetNavigationSpline().GetPoint(myCurrentSplineIndex);
	}

	CU::Vector2f splineNormal(-currentSpline->myForwardDirection.y, currentSpline->myForwardDirection.x);
	CU::Vector2f leftPoint(currentSpline->myPosition - splineNormal * (currentSpline->myWidth * 0.5f));
	CU::Vector2f rightPoint(currentSpline->myPosition + splineNormal * (currentSpline->myWidth * 0.5f));


	//CU::Vector2f lineVector = rightPoint - leftPoint;
	//CU::Vector2f distance = pos - leftPoint;

	//lineVector.Normalize();
	//float shadowLength = distance.Dot(lineVector);



	CU::Vector2f closestPoint = /*lineVector * shadowLength*/pDistance(pos.x, pos.y, leftPoint.x, leftPoint.y, rightPoint.x, rightPoint.y);


	//CU::Vector2f toClosestPointDirection = closestPoint - pos;
	//toClosestPointDirection.Normalize();
	//CU::Vector2f toClosestPointDirectionNormal(-toClosestPointDirection.y, toClosestPointDirection.x);
	//toClosestPointDirectionNormal.Normalize();
	//toClosestPointDirectionNormal = splineNormal;
	//closestPoint = currentSpline->myPosition;
	CU::Vector3f closestPoint3D(closestPoint.x, 0.f, closestPoint.y);


	CU::Matrix44f newRotation = myControllerComponent.GetParent()->GetToWorldTransform();
	newRotation.LookAt(closestPoint3D);
	CU::Matrix44f rotationDifference = newRotation * myControllerComponent.GetParent()->GetToWorldTransform().GetRotation().GetTransposed();


	float turnAmount = -rotationDifference.GetEulerRotation().y;
	//DL_PRINT("turnAmount: %f", turnAmount);

	float turnAmountAbs = std::fabs(turnAmount);
	if ((turnAmountAbs < 0.2f) ||
		(myControllerComponent.GetVelocity().Length2() < 500.f))
	{
		myState = eStates::eForward;
	}
	else
	{
		myState = eStates::eStop;
	}

	switch (myState)
	{
	case CAIController::eStates::eForward:
		myControllerComponent.MoveFoward();
		break;
	case CAIController::eStates::eStop:
		myControllerComponent.StopMoving();
		break;
	case CAIController::eStates::eBackward:
		myControllerComponent.MoveBackWards();
		if (forward.Dot(myControllerComponent.GetVelocity().GetNormalized()) < 0.0f)
		{
			turnAmount = -signof(turnAmount);
		}
		break;
	}
	myControllerComponent.Turn(CLAMP(turnAmount, -1.f, 1.0f));

	if (turnAmountAbs > 0.2f)
	{
		if (myIsDrifting == 0)
		{
			//DL_PRINT("turn %f, drift %d", turnAmount, (int)myIsDrifting);
			if (myControllerComponent.Drift())
			{
				myIsDrifting = static_cast<char>(signof(turnAmount));
			}
		}
	}
	else if (myIsDrifting != 0)
	{
		myControllerComponent.StopDrifting();
		myIsDrifting = 0;
	}
}

const CNavigationSpline& CAIController::GetNavigationSpline()
{
	return myControllerComponent.GetNavigationSpline();
}
