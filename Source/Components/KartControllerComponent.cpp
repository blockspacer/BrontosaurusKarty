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

CKartControllerComponent::CKartControllerComponent(): myFallSpeed(0)
{
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

void CKartControllerComponent::Update(const float aDeltaTime)
{
	DoPhysics(aDeltaTime);

	float way = 1.f;
	if (myFowrardSpeed > 0.f)
	{
		way = -1.f;
	}

	myFowrardSpeed += myFriction * way * aDeltaTime;

	myFowrardSpeed += myAcceleration * aDeltaTime * myAccelerationModifier;
	if (myFowrardSpeed > myMaxSpeed * myMaxSpeedModifier)
	{
		myFowrardSpeed -= myBoostSpeedDecay * aDeltaTime;
	}
	if (myFowrardSpeed < myMinSpeed)
	{
		myFowrardSpeed = myMinSpeed;
	}

	float steerAngle = (mySteering + myDriftSteerModifier) * myAngularAcceleration * -way;
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

void CKartControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
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

const float gravity = 9.82;
void CKartControllerComponent::DoPhysics(const float aDeltaTime)
{
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f pos = GetParent()->GetWorldPosition();
	//Update fall speed
	myFallSpeed += gravity * aDeltaTime;

	//Check if on ground
	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(pos, down, 1);
	if(raycastHitData.hit == true && raycastHitData.distance < 0.02)
	{
		myFallSpeed = 0;
	}


	//When not on ground, do fall
	const CU::Vector3f disp = down * myFallSpeed * aDeltaTime;
	GetParent()->GetLocalTransform().Move(disp);
}
