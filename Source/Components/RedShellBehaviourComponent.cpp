#include "stdafx.h"
#include "RedShellBehaviourComponent.h"
#include "../Physics/PhysicsScene.h"
#include "../CommonUtilities/JsonValue.h"
#include "..\Game\NavigationSpline.h"
#include "..\CommonUtilities\line.h"
#include "KartControllerComponentManager.h"



CU::Vector2f pDistance(const float x, const float y, const float x1, const float y1, const float x2, const float y2);


CRedShellBehaviourComponent::CRedShellBehaviourComponent()
{
	CU::CJsonValue boostList;
	std::string filePath = "Json/Items.json";
	const std::string& errorString = boostList.Parse(filePath);
	CU::CJsonValue levelsArray = boostList.at("Items");
	CU::CJsonValue Item = levelsArray.at("GreenShell");

	Speed = 10;//Item.at("MaxSpeed").GetFloat();

	myVelocity = CU::Vector3f::UnitZ*	Speed;

	myCurrentSplineIndex = 0;
}


CRedShellBehaviourComponent::~CRedShellBehaviourComponent()
{
}

void CRedShellBehaviourComponent::Init(Physics::CPhysicsScene * aPhysicsScene, CKartControllerComponentManager* aKartManager)
{
	myPhysicsScene = aPhysicsScene;
	myKartManager = aKartManager;
}

void CRedShellBehaviourComponent::Update(const float aDeltaTime)
{
	if (myKartManager->GetNavigationSpline().GetNumberOfPoints() == 0)
	{
		return;
	}

	DoPhysics(aDeltaTime);

	CU::Vector3f piss = GetParent()->GetToWorldTransform().GetPosition();
	CU::Vector2f pos(piss.x, piss.z);

	CU::Vector3f forward = GetParent()->GetToWorldTransform().myForwardVector;
	CU::Vector2f kartForward(forward.x, forward.z);
	kartForward.Normalize();
	pos += kartForward * 5.f;

	const SNavigationPoint * currentSpline = &myKartManager->GetNavigationSpline().GetPoint(myCurrentSplineIndex);


	if (currentSpline->myForwardDirection.Dot(currentSpline->myPosition - pos) < 0.0f) //&& IsGrounded;
	{
		myCurrentSplineIndex++;
		myCurrentSplineIndex %= myKartManager->GetNavigationSpline().GetNumberOfPoints();
		currentSpline = &myKartManager->GetNavigationSpline().GetPoint(myCurrentSplineIndex);
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


	CU::Matrix44f newRotation = GetParent()->GetToWorldTransform();
	newRotation.LookAt(closestPoint3D);
	CU::Matrix44f rotationDifference = newRotation * GetParent()->GetToWorldTransform().GetRotation().GetTransposed();

	newRotation.Move(myVelocity*aDeltaTime);

	GetParent()->SetWorldTransformation(newRotation);

	SComponentMessageData data; data.myFloat = aDeltaTime;
	GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, data);

}

const float gravity = 9.82f * 10;
const float upDistConst = 0.01f;
const float testLength = 2.f;

void CRedShellBehaviourComponent::DoPhysics(const float aDeltaTime)
{
	const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f upMove = CU::Vector3f::UnitY;
	const float upMoveLength = upMove.Length();
	const float upDist = upDistConst + upMoveLength;
	const float onGroundDist = upDistConst * 2.f + upMoveLength;
	const float controlDist = upDistConst * 100.f + upMoveLength;
	const CU::Vector3f pos = transformation.GetPosition();


	//Update fall speed per wheel

	CU::Vector3f examineVector = pos;

	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector + upMove, down, testLength, Physics::eGround);

	CU::Vector3f downAccl = down;
	if (raycastHitData.hit == true)
	{
		const CU::Vector3f& norm = raycastHitData.normal;

		if (raycastHitData.distance > onGroundDist)
		{
			const float disp = upDist - raycastHitData.distance;
			downAccl = CU::Vector3f::Zero;
			myVelocity.y = 0;

			GetParent()->GetLocalTransform().Move(-down * (disp > 0.f ? 0.f : disp));
		}
		else if (raycastHitData.distance < upDist)
		{
			//const float speed = myVelocity.Length();
			//myVelocity.y = 0.f; downAccl * speed;
			downAccl = CU::Vector3f::Zero;
			const float disp = upDist - raycastHitData.distance;
			myVelocity.y = 0;

			GetParent()->GetLocalTransform().Move(-down * (disp < 0.f ? 0.f : disp));
		}
	}


	myVelocity += downAccl  *gravity * aDeltaTime;
}

void CRedShellBehaviourComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
}


