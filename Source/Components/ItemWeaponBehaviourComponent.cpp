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

	Speed = Item.at("MaxSpeed").GetFloat();

	myVelocity = CU::Vector3f::UnitZ*	Speed;
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
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, messageData);

		
	}
}

const float gravity = 9.82f;
const float upDistConst = 0.01f;
const float testLength = 2.f;

void CItemWeaponBehaviourComponent::DoPhysics(const float aDeltaTime)
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

		if (raycastHitData.distance < upDist)
		{
			const float speed = myVelocity.Length();
			//myVelocity.y = 0.f; downAccl * speed;

			const float disp = upDist - raycastHitData.distance;

			GetParent()->GetLocalTransform().Move(norm * (disp < 0.f ? 0.f : disp));
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
	case eComponentMessageType::eActivate:
	{
		myIsActive = true;
		myVelocity = CU::Vector3f::UnitZ*Speed;
		break;
	}

	}
}
