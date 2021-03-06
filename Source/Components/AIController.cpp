#include "stdafx.h"
#include "AIController.h"
#include "KartControllerComponent.h"
#include "..\Game\NavigationSpline.h"
#include "..\CommonUtilities\line.h"

#include "AIMath.h"


CAIController::CAIController(CKartControllerComponent& aKartComponent)
	: CController(aKartComponent)
{
	myCurrentSplineIndex = 0;
	myUseItemTimer = 0.f;
	myState = eStates::eStop;
	myIsDrifting = 0;
	myHasItem = false;
	myCollisionCheckCountdown = 5.0f;
}

CAIController::~CAIController()
{
}

void CAIController::Update(const float aDeltaTime)
{
	UpdateItemUsage(aDeltaTime);
	UpdateMovement(aDeltaTime, 1.f);
	UpdateUnstuck(aDeltaTime);
}

void CAIController::UpdateWithoutItems(const float aDeltaTime)
{
	UpdateMovement(aDeltaTime, 0.75f);
}

void CAIController::UpdateItemUsage(const float aDeltaTime)
{
	if (myHasItem == false)
	{
		if (myControllerComponent.GetParent()->AskComponents(eComponentQuestionType::eGetHoldItemType, SComponentQuestionData()) == true)
		{
			myHasItem = true;
			myUseItemTimer = 2.f;
			myControllerComponent.GetParent()->NotifyOnlyComponents(eComponentMessageType::eUseItem, SComponentMessageData());
		}
	}
	else
	{
		myUseItemTimer -= aDeltaTime;
		if (myUseItemTimer <= 0.f)
		{
			myHasItem = false;
			myControllerComponent.GetParent()->NotifyOnlyComponents(eComponentMessageType::eReleaseItem, SComponentMessageData());
		}
	}
}

void CAIController::UpdateMovement(const float aDeltaTime, const float aMaxSpeedModifier)
{
	const CU::Vector3f pos3D = myControllerComponent.GetParent()->GetToWorldTransform().GetPosition();
	CU::Vector2f pos(pos3D.xz());

	const CU::Vector3f forward = myControllerComponent.GetParent()->GetToWorldTransform().myForwardVector;
	CU::Vector2f kartForward(forward.x, forward.z);
	kartForward.Normalize();
	pos += kartForward * 5.f;

	const SNavigationPoint * currentSpline = &GetNavigationSpline().GetPoint(myCurrentSplineIndex);


	if (currentSpline->myForwardDirection.Dot(currentSpline->myPosition - pos) < 0.0f
		&& myControllerComponent.GetIsGrounded())
	{
		myCurrentSplineIndex++;
		const short numPoints = GetNavigationSpline().GetNumberOfPoints();
		myCurrentSplineIndex %= numPoints;
		currentSpline = &GetNavigationSpline().GetPoint(myCurrentSplineIndex);
	}

	CU::Vector2f splineNormal(-currentSpline->myForwardDirection.y, currentSpline->myForwardDirection.x);
	CU::Vector2f leftPoint(currentSpline->myPosition - splineNormal * (currentSpline->myWidth * 0.5f));
	CU::Vector2f rightPoint(currentSpline->myPosition + splineNormal * (currentSpline->myWidth * 0.5f));

	CU::Vector2f closestPoint = AIMath::pDistance(pos.x, pos.y, leftPoint.x, leftPoint.y, rightPoint.x, rightPoint.y);
	CU::Vector3f closestPoint3D(closestPoint.x, 0.f, closestPoint.y);

	CU::Matrix44f newRotation = myControllerComponent.GetParent()->GetToWorldTransform();
	newRotation.LookAt(closestPoint3D);
	CU::Matrix44f rotationDifference = newRotation * myControllerComponent.GetParent()->GetToWorldTransform().GetRotation().GetTransposed();

	float turnAmount = -rotationDifference.GetEulerRotation().y;

	float turnAmountAbs = std::fabs(turnAmount);
	if (turnAmountAbs < 0.2f * 30.f && myControllerComponent.GetVelocity().xz().Length2() < myControllerComponent.GetMaxSpeed2() * aMaxSpeedModifier)
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
			turnAmount = -AIMath::signof(turnAmount);
		}
		break;
	}
	myControllerComponent.Turn(CLAMP(turnAmount, -1.f, 1.0f));

	if (turnAmountAbs > 0.2f)
	{
		if (myIsDrifting == 0)
		{
			if (myControllerComponent.Drift())
			{
				myIsDrifting = static_cast<char>(AIMath::signof(turnAmount));
			}
		}
	}
	else if (myIsDrifting != 0)
	{
		myControllerComponent.StopDrifting(true);
		myIsDrifting = 0;
	}
}

const CNavigationSpline& CAIController::GetNavigationSpline()
{
	return myControllerComponent.GetNavigationSpline();
}

void CAIController::UpdateUnstuck(const float aDeltaTime)
{
	if(myCollisionCheckCountdown > 0.0f)
	{
		myCollisionCheckCountdown -= aDeltaTime;
		if (myCollisionCheckCountdown <= 0.0f)
		{
			myCollisionCheckCountdown = 5.0f;
			float distance = CU::Vector3f(myCollisionPosition - myControllerComponent.GetParent()->GetWorldPosition()).Length2();
			if(distance < 10.0f * 10.0f)
			{
				myControllerComponent.GetParent()->NotifyOnlyComponents(eComponentMessageType::eInflictZKill, SComponentMessageData());
			}
			myCollisionPosition = myControllerComponent.GetParent()->GetWorldPosition();
		}
	}
}