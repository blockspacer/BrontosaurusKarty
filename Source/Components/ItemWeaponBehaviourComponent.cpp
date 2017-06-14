#include "stdafx.h"
#include "ItemWeaponBehaviourComponent.h"
#include "../Physics/PhysicsScene.h"
#include "../CommonUtilities/JsonValue.h"


CItemWeaponBehaviourComponent::CItemWeaponBehaviourComponent()
{
	myIsActive = false;
	myIsOnGround = false;
	myGrip = 16;
	myPhysicsScene = nullptr;

	CU::CJsonValue boostList;
	std::string filePath = "Json/Items.json";
	const std::string& errorString = boostList.Parse(filePath);
	CU::CJsonValue levelsArray = boostList.at("Items");
	CU::CJsonValue Item = levelsArray.at("GreenShell");

	mySpeed = Item.at("MaxSpeed").GetFloat();

	myVelocity = CU::Vector3f::UnitZ * mySpeed;
	myWeight = 1.5f;
	
}


CItemWeaponBehaviourComponent::~CItemWeaponBehaviourComponent()
{
}

void CItemWeaponBehaviourComponent::Init(Physics::CPhysicsScene * aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
	myIsActive = false;
}

void CItemWeaponBehaviourComponent::SetToNoSpeed()
{
	mySpeed = 0;
	myVelocity = CU::Vector3f::Zero;
}

void CItemWeaponBehaviourComponent::Update(const float aDeltaTime)
{
	if (myIsActive == true)
	{
		DoPhysics(aDeltaTime);

		CU::Matrix44f worldTransform = GetParent()->GetToWorldTransform();

		worldTransform.Move(myVelocity*aDeltaTime);

		GetParent()->SetWorldTransformation(worldTransform);


		SComponentMessageData messageData;
		messageData.myFloat = aDeltaTime;
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, messageData);

		
	}
}

const float gravity = 9.82f*10;
const float upDistConst = 0.01f;
const float testLength = 2.f;

void CItemWeaponBehaviourComponent::DoPhysics(const float aDeltaTime)
{


	const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
	const CU::Vector3f forward = transformation.myForwardVector.GetNormalized();
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f upMove = CU::Vector3f::UnitY;
	const float upMoveLength = upMove.Length();
	const float upDist = upDistConst + upMoveLength;
	const float onGroundDist = upDistConst * 2.f + upMoveLength;
	const float controlDist = upDistConst * 100.f + upMoveLength;
	const CU::Vector3f pos = transformation.GetPosition();

	const CU::Vector3f velInWorld = myVelocity * transformation.GetRotation();
	Physics::SRaycastHitData  raycastHitData = myPhysicsScene->Raycast(pos, velInWorld.GetNormalized(), testLength, Physics::eWall);
	const float speed = myVelocity.Length();
	if (raycastHitData.hit && raycastHitData.distance < speed * aDeltaTime)
	{
		const CU::Vector3f& norm = raycastHitData.normal;
		const CU::Vector3f invVel = -velInWorld;
		const float dotNorm = invVel.Dot(norm);
		const CU::Vector3f longNorm = norm * dotNorm;
		const CU::Vector3f toNorm = velInWorld + longNorm;
		myVelocity = (((invVel + 2.f * toNorm)) * transformation.GetRotation().GetInverted()).Normalize() * speed;
		
	}

	//Update fall speed per wheel

	CU::Vector3f examineVector = pos;

	raycastHitData = myPhysicsScene->Raycast(examineVector + upMove, down, testLength, Physics::eGround);

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

void CItemWeaponBehaviourComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{

	case eComponentMessageType::eDeactivate:
	{
		myIsActive = false;
		break;
	}
	case eComponentMessageType::eReInitItem:
	{

		myIsActive = true;
		float velocitiesSpeed = mySpeed;
		if(mySpeed > 0.0f)
		{
			SComponentQuestionData speedQuestionData;
			if (aMessageData.myComponent->GetParent()->AskComponents(eComponentQuestionType::eGetMaxSpeed, speedQuestionData) == true)
			{
				if (velocitiesSpeed < speedQuestionData.myFloat)
				{
					velocitiesSpeed = speedQuestionData.myFloat + 5;
				}
			}
		}
		myVelocity = CU::Vector3f::UnitZ * velocitiesSpeed;
		break;
	}

	}
}
