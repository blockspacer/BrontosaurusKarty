#include "stdafx.h"
#include "KartControllerComponent.h"
#include "SpeedHandlerManager.h"
#include "BoostData.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"
#include "../CommonUtilities/JsonValue.h"
#include "../PostMaster/SetVibrationOnController.h"
#include "../PostMaster/StopVibrationOnController.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../Physics/PhysicsCallbackActor.h"
#include "../Physics/PhysXManager.h"
#include "../Physics/PhysicsScene.h"
#include "../TServer/GameServer.h"
#include "CommonUtilities.h"


CKartControllerComponent::CKartControllerComponent(): myPhysicsScene(nullptr), myFirstMovingPass(true)
{
	ClearHeight();
	ClearSpeed();
	

	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/KartStats.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("Karts");

	CU::CJsonValue Karts = levelsArray.at("BaseKart");


	myFowrardSpeed = 0.0f;
	myMaxSpeed = Karts.at("MaxSpeed").GetFloat();
	myMinSpeed = Karts.at("ReverseSpeed").GetFloat();
	myAcceleration = 0.0f;

	myMaxAcceleration = Karts.at("MaxAcceleration").GetFloat();
	myMinAcceleration = Karts.at("ReverseAcceleration").GetFloat();

	myTurnRate = Karts.at("TurnRate").GetFloat();

	myFriction = 10.f;

	mySteering = 0.f;
	myAngularAcceleration = Karts.at("AngulareAcceleration").GetFloat();

	myMaxSpeedModifier = 1.0f;
	myAccelerationModifier = 1.0f;

	myIsDrifting = false;
	myDriftRate = 0;
	myDriftTimer = 0;
	myDriftSteerModifier = 0;
	myDriftSteeringModifier = Karts.at("DriftTurnRate").GetFloat();
	myMaxDriftRate = Karts.at("MaxDriftRate").GetFloat();
	myTimeMultiplier = Karts.at("DriftOverTimeMultiplier").GetFloat();
	myMaxDriftSteerAffection = Karts.at("MaxDriftTurnRate").GetFloat();
	myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;

	myLeftWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
	myRightWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
	myLeftDriftBoostEmitterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("FirstStageBoostParticle").GetString());
	myRightDriftBoostEmitterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("FirstStageBoostParticle").GetString());

	myCurrentAction = eCurrentAction::eDefault;
}


CKartControllerComponent::~CKartControllerComponent()
{
}

void CKartControllerComponent::TurnRight()
{
	myCurrentAction = eCurrentAction::eTurningRight;
	if (myIsDrifting == false)
	{
		mySteering = myTurnRate;
	}
	else
	{
		myDriftSteerModifier = myDriftSteeringModifier;
	}
}

void CKartControllerComponent::TurnLeft()
{
	myCurrentAction = eCurrentAction::eTurningLeft;
	if (myIsDrifting == false)
	{
		mySteering = -myTurnRate;
	}
	else
	{
		myDriftSteerModifier = -myDriftSteeringModifier;
	}
}

void CKartControllerComponent::StopMoving()
{
	myAcceleration = 0;
}

void CKartControllerComponent::MoveFoward()
{
	myAcceleration = myMaxAcceleration;
}

void CKartControllerComponent::MoveBackWards()
{
	myAcceleration = myMinAcceleration;
}

void CKartControllerComponent::StopTurning()
{
	myCurrentAction = eCurrentAction::eDefault;
	if (myIsDrifting == false)
	{
		mySteering = 0;
	}
	else
	{
		myDriftSteerModifier = 0;
	}
}

//Checks if the player is turning left or right and then sets the drift values accordingly
void CKartControllerComponent::Drift()
{
	if (mySteering > 0)
	{
		myIsDrifting = true;
		myDriftRate = -myMaxDriftRate;
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
		SetVibrationOnController* vibrationMessage = new SetVibrationOnController(0, 10, 10);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
	}
	else if (mySteering < 0)
	{
		myIsDrifting = true;
		myDriftRate = myMaxDriftRate;
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
		SetVibrationOnController* vibrationMessage = new SetVibrationOnController(0, 10, 10);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
	}
}

void CKartControllerComponent::StopDrifting()
{
	CParticleEmitterManager::GetInstance().Deactivate(myLeftWheelDriftEmmiterHandle);
	CParticleEmitterManager::GetInstance().Deactivate(myRightWheelDriftEmmiterHandle);

	if (myDriftTimer >= 2.0f)
	{
		if (myDriftTimer >= 5.0f)
		{
			SComponentMessageData boostMessageData;
			boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("DriftBoost"));
			GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		}
		else
		{
			SComponentMessageData boostMessageData;
			boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("MiniDriftBoost"));
			GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		}
	}

	myIsDrifting = false;
	myDriftRate = 0;
	myDriftTimer = 0;
	myDriftSteerModifier = 0;
	CParticleEmitterManager::GetInstance().Deactivate(myLeftDriftBoostEmitterhandle);
	CParticleEmitterManager::GetInstance().Deactivate(myRightDriftBoostEmitterhandle);

	StopVibrationOnController* stopionMessageLeft = new StopVibrationOnController(0);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(stopionMessageLeft);

	switch (myCurrentAction)
	{
	case CKartControllerComponent::eCurrentAction::eTurningRight:
		TurnRight();
		break;
	case CKartControllerComponent::eCurrentAction::eTurningLeft:
		TurnLeft();
		break;
	case CKartControllerComponent::eCurrentAction::eDefault:
		StopTurning();
		break;
	default:
		break;
	}
}

//TODO: Hard coded, not good, change soon
const float killHeight = -25;

void CKartControllerComponent::CheckZKill()
{
	const float height = GetParent()->GetWorldPosition().y;

	if(height < killHeight)
	{
		GetParent()->SetWorldTransformation(CU::Matrix44f());
		GetParent()->SetWorldPosition(CU::Vector3f(0.f, 1.f, 0.f));
		myFowrardSpeed = 0.f;
		for(int i  = 0; i < static_cast<int>(AxisPos::Size); ++i)
		{
			myAxisSpeed[i] = 0;
		}
		ClearSpeed();
		ClearHeight();
	}
}

void CKartControllerComponent::Update(const float aDeltaTime)
{
	DoPhysics(aDeltaTime);
	CheckZKill();

	float way = 1.f;
	if (myFowrardSpeed > 0.f)
	{
		way = -1.f;
	}

	const float onGroundModifier = (myIsOnGround == true ? 1.f : 0.f);
	myFowrardSpeed += myFriction * way * aDeltaTime * onGroundModifier;
	myFowrardSpeed += myAcceleration * aDeltaTime * myAccelerationModifier * onGroundModifier;
	if (myFowrardSpeed > myMaxSpeed * myMaxSpeedModifier)
	{
		myFowrardSpeed -= myBoostSpeedDecay * aDeltaTime;
	}
	if (myFowrardSpeed < myMinSpeed)
	{
		myFowrardSpeed = myMinSpeed;
	}

	float steerAngle = (mySteering  + myDriftSteerModifier) * myAngularAcceleration * -way * onGroundModifier;
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	parentTransform.RotateAroundAxis(steerAngle * aDeltaTime, CU::Axees::Y);

	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myFowrardSpeed * aDeltaTime));
	if (myIsDrifting == true)
	{
		myDriftTimer += aDeltaTime;
		if (mySteering > 0)
		{
			if (mySteering <= myMaxDriftSteerAffection)
			{
				mySteering += (aDeltaTime / myTimeMultiplier);
			}
		}
		else if (mySteering < 0)
		{
			if (mySteering >= -myMaxDriftSteerAffection)
			{
				mySteering -= (aDeltaTime / myTimeMultiplier);
			}
		}
		GetParent()->GetLocalTransform().Move(CU::Vector3f(myDriftRate * aDeltaTime, 0.0f, 0.0f));
		CU::Matrix44f particlePosition = GetParent()->GetLocalTransform();
		particlePosition.Move(CU::Vector3f(-0.45f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myLeftWheelDriftEmmiterHandle, particlePosition.GetPosition());
		CParticleEmitterManager::GetInstance().SetPosition(myLeftDriftBoostEmitterhandle, particlePosition.GetPosition());
		particlePosition.Move(CU::Vector3f(0.9f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myRightWheelDriftEmmiterHandle, particlePosition.GetPosition());
		CParticleEmitterManager::GetInstance().SetPosition(myRightDriftBoostEmitterhandle, particlePosition.GetPosition());
		static bool driftParticlesActivated = false;
		if (myDriftTimer >= 2.0f && driftParticlesActivated == false)
		{
			CParticleEmitterManager::GetInstance().Activate(myLeftDriftBoostEmitterhandle);
			CParticleEmitterManager::GetInstance().Activate(myRightDriftBoostEmitterhandle);
		}
	}
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CKartControllerComponent::ClearHeight()
{
	

	for(int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		ClearHeight(i);
	}
}

void CKartControllerComponent::ClearHeight(const int anIndex)
{
	float height = 0.f;
	if (GetParent() != nullptr)
	{
		height = GetParent()->GetWorldPosition().y;
	}
	SetHeight(anIndex, height, 1.f);
	SetHeight(anIndex, height, 1.f);
}

void CKartControllerComponent::SetHeight(int aWheelIndex, float aHeight, const float aDt)
{
	myPreviousHeight[aWheelIndex] = myCurrentHeight[aWheelIndex];
	myCurrentHeight[aWheelIndex] = aHeight;
}

float CKartControllerComponent::GetHeightSpeed(int anIndex)
{
	const float heightDelta = myCurrentHeight[anIndex] - myPreviousHeight[anIndex];

	if(heightDelta < 0.f)
	{
		return 0.f;
	}

	if(heightDelta > 0.f)
	{
		int i = 0;
	}

	return heightDelta;
}

void CKartControllerComponent::ApplyNormalityBias(const float aDt)
{
}

void CKartControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if(aMessageData.myComponent == this)
		{
			ClearHeight();
		}
	
		break;
	case eComponentMessageType::eObjectDone:
		if(myFirstMovingPass == true)
		{
			myFirstMovingPass = false;
			ClearHeight();
		}
		break;

	case eComponentMessageType::eSetBoost:
	{
		myMaxSpeedModifier = 1.0f + aMessageData.myBoostData->maxSpeedBoost;
		myAccelerationModifier = 1.0f + aMessageData.myBoostData->accerationBoost;
		myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;
	}
	default:
		break;
	}

}

void CKartControllerComponent::Init(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

void CKartControllerComponent::ClearSpeed()
{
	for (int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		myAxisSpeed[i] = 0.f;
	}
}

const float gravity = 9.82;
const float upDist = 0.05;
void CKartControllerComponent::DoPhysics(const float aDeltaTime)
{
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f right = GetParent()->GetToWorldTransform().myRightVector;
	const CU::Vector3f front = GetParent()->GetToWorldTransform().myForwardVector;
	const CU::Vector3f pos = GetParent()->GetWorldPosition();

	const float halfWidth = myAxisDescription.width / 2.f;
	const float halfLength = myAxisDescription.length / 2.f;

	const CU::Vector3f rxhw = halfWidth * right;

	

	//Check if on ground (once per axis)

	CU::Vector3f axees[static_cast<int>(AxisPos::Size)];

	myIsOnGround = false;

	for(int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		//Update fall speed
		
		CU::Vector3f examineVector = pos;

		if(i % 2 != 0)
		{
			examineVector += front * myAxisDescription.length;
		}
		if(i < 2)
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
			if(raycastHitData.distance < upDist * 2.f)
			{
				myIsOnGround = true;

				SetHeight(i, examineVector.y, aDeltaTime);
			}
			if(raycastHitData.distance < upDist)
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

		if(heightSpeed > 0.f)
		{
			int i = 0; 

		}

		//myAxisSpeed[i] -= CLAMP(heightSpeed, 0.f, 1000.f);

		//When not on ground, do fall
		const CU::Vector3f disp = down * myAxisSpeed[i] * aDeltaTime/* + CU::Vector3f::UnitY * heightSpeed * aDeltaTime*/;

		
		examineVector += disp;

		axees[i] = examineVector;

		
	}

	CU::Vector3f avgPos = CU::Vector3f::Zero;

	for(int i = 0; i < static_cast<int>(AxisPos::Size);++i)
	{
		avgPos += axees[i];
	}

	avgPos /= static_cast<int>(AxisPos::Size);

	avgPos -= front * halfLength;

	CU::Matrix44f transform = GetParent()->GetToWorldTransform();

	const CU::Vector3f avgRVec = (axees[static_cast<int>(AxisPos::RightFront)] + axees[static_cast<int>(AxisPos::RightBack)]) / 2.f;
	const CU::Vector3f avgLVec = (axees[static_cast<int>(AxisPos::LeftFront)] + axees[static_cast<int>(AxisPos::LeftBack)]) / 2.f;

	const CU::Vector3f avgFVec = (axees[static_cast<int>(AxisPos::RightFront)] + axees[static_cast<int>(AxisPos::LeftFront)]) / 2.f;
	const CU::Vector3f avgBVec = (axees[static_cast<int>(AxisPos::RightBack)] + axees[static_cast<int>(AxisPos::LeftBack)]) / 2.f;

	const CU::Vector3f newRight = (avgRVec - avgLVec).Normalize();
	CU::Vector3f newFront = (avgFVec - avgBVec).Normalize();

	const CU::Vector3f newUp = newFront.Cross(newRight).Normalize();

	newFront = newRight.Cross(newUp);

	CU::Matrix33f newRotation;
	newRotation.myRightVector = newRight;
	newRotation.myForwardVector = newFront;
	newRotation.myUpVector = newUp;

	
	
	if(myIsOnGround == false)
	{
		//ApplyNormalityBias(aDeltaTime);
	}

	transform.SetRotation(newRotation);

	transform.SetPosition(avgPos);
	GetParent()->SetWorldTransformation(transform);
	NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
}
