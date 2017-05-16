#include "stdafx.h"
#include "KartModelComponent.h"
#include "CommonUtilities.h"
#include "../Physics/PhysicsScene.h"
#include "../Physics/PhysXHelper.h"
#include <foundation/PxQuat.h>


Component::CKartModelComponent::CKartModelComponent(Physics::CPhysicsScene* aPhysicsScene): myPhysicsScene(aPhysicsScene)
{
	ClearHeight();
	ClearSpeed();
}


Component::CKartModelComponent::~CKartModelComponent()
{
}

void Component::CKartModelComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CComponent::Receive(aMessageType, aMessageData);

	switch (aMessageType)
	{
	case eComponentMessageType::eUpdate:
		Update(aMessageData.myFloat);
		break;
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponent == this)
		{
			ClearHeight();
		}
		break;
	case eComponentMessageType::eObjectDone:
		//if (myFirstMovingPass == true)
		{
			//myFirstMovingPass = false;
			ClearHeight();
		}
		break;
	case eComponentMessageType::eKill:
		Reset();
		break;
	}
}

bool Component::CKartModelComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	return false;
}

void Component::CKartModelComponent::Destroy()
{
	delete this;
}

void Component::CKartModelComponent::ClearHeight()
{
	for (int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		ClearHeight(i);
	}
}

void Component::CKartModelComponent::ClearHeight(const int anIndex)
{
	float height = 0.f;
	if (GetParent() != nullptr)
	{
		height = GetParent()->GetWorldPosition().y;
	}
	SetHeight(anIndex, height, 1.f);
	SetHeight(anIndex, height, 1.f);
}

void Component::CKartModelComponent::ClearSpeed()
{
	for (int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		myAxisSpeed[i] = 0.f;
	}
}

float Component::CKartModelComponent::GetHeightSpeed(int anIndex) const
{
	const float heightDelta = myCurrentHeight[anIndex] - myPreviousHeight[anIndex];

	if (heightDelta < 0.f)
	{
		return 0.f;
	}

	if (heightDelta > 0.f)
	{
		int i = 0;
	}

	return heightDelta;
}

void Component::CKartModelComponent::SetHeight(int anIndex, float aHeight, float aDeltaTime)
{
	myPreviousHeight[anIndex] = myCurrentHeight[anIndex];
	myCurrentHeight[anIndex] = aHeight;
}

const float sleprVal = 0.1f;

void Component::CKartModelComponent::NormalizeRotation(const float aDeltaTime)
{
	CU::Matrix44f result = Physics::Slerp(GetParent()->GetLocalTransform(), CU::Matrix44f::Identity, sleprVal * aDeltaTime);
	GetParent()->GetLocalTransform() = result;
}


const float gravity = 9.82f;
const float upDist = 0.5f;

void Component::CKartModelComponent::Update(const float aDeltaTime)
{

	NormalizeRotation(aDeltaTime);

	const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f right = transformation.myRightVector;
	const CU::Vector3f front = transformation.myForwardVector;
	const CU::Vector3f pos = transformation.GetPosition();

	const float halfWidth = myAxisDescription.width / 2.f;
	const float halfLength = myAxisDescription.length / 2.f;

	const CU::Vector3f rxhw = halfWidth * right;

	CU::Vector3f axees[static_cast<int>(AxisPos::Size)];

	myIsOnGround = false;

	for (int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		//Update fall speed per wheel

		CU::Vector3f examineVector = pos;

		if (i % 2 != 0)
		{
			examineVector += front * myAxisDescription.length;
		}
		if (i < 2)
		{
			examineVector += rxhw;
		}
		else
		{
			examineVector -= rxhw;
		}

		Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector, down, 1);

		const float heightSpeed = GetHeightSpeed(i);
		if (raycastHitData.hit == true)
		{
			if (raycastHitData.distance < upDist * 2.f)
			{
				myIsOnGround = true;

				SetHeight(i, examineVector.y, aDeltaTime);
			}
			if (raycastHitData.distance < upDist)
			{
				myAxisSpeed[i] = 0;

				const float disp = upDist - raycastHitData.distance;

				examineVector -= down * (disp < 0.f ? 0.f : disp);
			}
		}
		else
		{


			ClearHeight(i);
		}


		myAxisSpeed[i] += gravity * aDeltaTime;

		if (heightSpeed > 0.f)
		{
			int i = 0;

		}

		//When not on ground, do fall
		const CU::Vector3f disp = down * myAxisSpeed[i] * aDeltaTime/* + CU::Vector3f::UnitY * heightSpeed * aDeltaTime*/;


		examineVector += disp;

		axees[i] = examineVector;
	}

	//Find new rotation stuff
	const CU::Vector3f avgRVec = (axees[static_cast<int>(AxisPos::RightFront)] + axees[static_cast<int>(AxisPos::RightBack)]) / 2.f;
	const CU::Vector3f avgLVec = (axees[static_cast<int>(AxisPos::LeftFront)] + axees[static_cast<int>(AxisPos::LeftBack)]) / 2.f;

	const CU::Vector3f avgFVec = (axees[static_cast<int>(AxisPos::RightFront)] + axees[static_cast<int>(AxisPos::LeftFront)]) / 2.f;
	const CU::Vector3f avgBVec = (axees[static_cast<int>(AxisPos::RightBack)] + axees[static_cast<int>(AxisPos::LeftBack)]) / 2.f;

	CU::Vector3f newRight = (avgRVec - avgLVec).Normalize();
	const CU::Vector3f newFront = (avgFVec - avgBVec).Normalize();

	const CU::Vector3f newUp = newFront.Cross(newRight).Normalize();

	newRight = newUp.Cross(newFront).Normalize();

	CU::Matrix33f newRotation;
	newRotation.myRightVector = newRight;
	newRotation.myForwardVector = newFront;
	newRotation.myUpVector = newUp;

	CU::Matrix44f transform;
	transform.SetRotation(newRotation);
	transform.SetPosition(GetParent()->GetWorldPosition());
	GetParent()->SetWorldTransformation(transform);

	//Set model rotation
	SComponentMessageData messageData;
	
	//if (myIsOnGround == false)
	//{
	//	rotationData.target = CU::Matrix33f::Identity;
	//	rotationData.stepSize = TAU / 10.f;
	//}
	//else
	//{
	//	rotationData.target = newRotation * CU::Matrix33f(GetParent()->GetToWorldTransform().GetRotation().GetInverted());
	//	rotationData.stepSize = TAU;
	//	messageData.myVoidPointer = &rotationData;

	//	NotifyParent(eComponentMessageType::eRotateTowards, messageData);
	//}
	////End

	//if(myIsOnGround == false)
	{
	}

	NotifyParent(eComponentMessageType::eMoving, messageData);
}

void Component::CKartModelComponent::Reset()
{
	for (int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		myAxisSpeed[i] = 0;
	}
	ClearSpeed();
	ClearHeight();
}
