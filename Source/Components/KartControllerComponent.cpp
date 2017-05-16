#include "stdafx.h"
#include "KartControllerComponent.h"
#include "SpeedHandlerManager.h"
#include "BoostData.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"
#include "Drifter.h"
#include "SmoothRotater.h"

#include "../CommonUtilities/JsonValue.h"

#include "../ThreadedPostmaster/SetVibrationOnController.h"
#include "../ThreadedPostmaster/StopVibrationOnController.h"
#include "../ThreadedPostmaster/Postmaster.h"

#include "../Physics/PhysicsCallbackActor.h"
#include "../Physics/PhysXManager.h"
#include "../Physics/PhysicsScene.h"

#include "../CommonUtilities/CommonUtilities.h"


CKartControllerComponent::CKartControllerComponent(): myPhysicsScene(nullptr), myMainSpeed(0)
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

	myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;

	myDrifter = std::make_unique<CDrifter>();
	myDrifter->Init(Karts);

	myLeftWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
	myRightWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
	myLeftDriftBoostEmitterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("FirstStageBoostParticle").GetString());
	myRightDriftBoostEmitterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("FirstStageBoostParticle").GetString());

	myCurrentAction = eCurrentAction::eDefault;
}


CKartControllerComponent::~CKartControllerComponent()
{
}

void CKartControllerComponent::Turn(float aDirectionX)
{
	if (aDirectionX < 0.f)
	{
		if (aDirectionX < -1.f)
		{
			aDirectionX = -1.f;
		}

		TurnLeft(aDirectionX);
	}
	else if (aDirectionX > 0.f)
	{
		if (aDirectionX > 1.f)
		{
			aDirectionX = 1.f;
		}

		TurnRight(aDirectionX);
	}
}

void CKartControllerComponent::TurnRight(const float aNormalizedModifier)
{
	assert(aNormalizedModifier <= 1.f && aNormalizedModifier >= -1.f && "normalized modifier not normalized mvh carl");
	myCurrentAction = eCurrentAction::eTurningRight;

	if (myDrifter->IsDrifting() == false)
	{
		mySteering = myTurnRate;
	}
	else
	{
		myDrifter->TurnRight();
	}
}

void CKartControllerComponent::TurnLeft(const float aNormalizedModifier)
{
	myCurrentAction = eCurrentAction::eTurningLeft;
	if (myDrifter->IsDrifting() == false)
	{
		mySteering = -myTurnRate;
	}
	else
	{
		myDrifter->TurnLeft();
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
	if (myDrifter->IsDrifting() == false)
	{
		mySteering = 0;
	}
	else
	{
		myDrifter->StopTurning();
	}
}

//Checks if the player is turning left or right and then sets the drift values accordingly
void CKartControllerComponent::Drift()
{
	myDrifter->StartDrifting(mySteering);
	if (mySteering > 0)
	{
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
		//SetVibrationOnController* vibrationMessage = new SetVibrationOnController(0, 10, 10);
		//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
	}
	else if (mySteering < 0)
	{
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
		//SetVibrationOnController* vibrationMessage = new SetVibrationOnController(0, 10, 10);
		//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
	}
}

void CKartControllerComponent::StopDrifting()
{
	CParticleEmitterManager::GetInstance().Deactivate(myLeftWheelDriftEmmiterHandle);
	CParticleEmitterManager::GetInstance().Deactivate(myRightWheelDriftEmmiterHandle);

	CDrifter::eDriftBoost boost = myDrifter->StopDrifting();

	SComponentMessageData boostMessageData;
	switch (boost)
	{
	case CDrifter::eDriftBoost::eLarge:
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("DriftBoost"));
		GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		break;
	case  CDrifter::eDriftBoost::eSmall:
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("MiniDriftBoost"));
		GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		break;
	case CDrifter::eDriftBoost::eNone:
		break;
	}

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
const float killHeight = -50;

void CKartControllerComponent::CheckZKill()
{
	const float height = GetParent()->GetWorldPosition().y;

	if(height < killHeight)
	{
		GetParent()->SetWorldTransformation(CU::Matrix44f());
		GetParent()->SetWorldPosition(CU::Vector3f(0.f, 1.f, 0.f));
		myFowrardSpeed = 0.f;
		GetParent()->NotifyComponents(eComponentMessageType::eKill, SComponentMessageData());
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

	float steerAngle = (mySteering + /*myDrifting.myDriftSteerModifier*/myDrifter->GetSteerModifier()) * myAngularAcceleration * -way * onGroundModifier;
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	parentTransform.RotateAroundAxis(steerAngle * aDeltaTime, CU::Axees::Y);

	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myFowrardSpeed * aDeltaTime));

	if (myDrifter->IsDrifting())
	{
		myDrifter->ApplySteering(mySteering, aDeltaTime);

		GetParent()->GetLocalTransform().Move(CU::Vector3f(/*myDrifting.myDriftRate*/myDrifter->GetDriftRate()  * aDeltaTime, 0.0f, 0.0f));

		CU::Matrix44f particlePosition = GetParent()->GetLocalTransform();

		particlePosition.Move(CU::Vector3f(-0.45f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myLeftWheelDriftEmmiterHandle, particlePosition.GetPosition());
		CParticleEmitterManager::GetInstance().SetPosition(myLeftDriftBoostEmitterhandle, particlePosition.GetPosition());
		particlePosition.Move(CU::Vector3f(0.9f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myRightWheelDriftEmmiterHandle, particlePosition.GetPosition());
		CParticleEmitterManager::GetInstance().SetPosition(myRightDriftBoostEmitterhandle, particlePosition.GetPosition());

		static bool driftParticlesActivated = false;
		if (myDrifter->WheelsAreBurning() && driftParticlesActivated == false)
		{
			CParticleEmitterManager::GetInstance().Activate(myLeftDriftBoostEmitterhandle);
			CParticleEmitterManager::GetInstance().Activate(myRightDriftBoostEmitterhandle);
		}
	}
	SComponentMessageData messageData;
	messageData.myFloat = aDeltaTime;
	GetParent()->NotifyComponents(eComponentMessageType::eUpdate, messageData);
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, messageData);
}

void CKartControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		break;
	case eComponentMessageType::eObjectDone:
		break;
	case eComponentMessageType::eSetBoost:
		myMaxSpeedModifier = 1.0f + aMessageData.myBoostData->maxSpeedBoost;
		myAccelerationModifier = 1.0f + aMessageData.myBoostData->accerationBoost;
		myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;
	}
}

void CKartControllerComponent::Init(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

const float gravity = 9.82f;
const float upDistConst = 0.01f;
const float testLength = 2.f;
void CKartControllerComponent::DoPhysics(const float aDeltaTime)
{
	const CU::Matrix44f transformation = GetParent()->GetToWorldTransform();
	const CU::Vector3f down = -CU::Vector3f::UnitY;
	const CU::Vector3f upMove = CU::Vector3f::UnitY;
	const float upMoveLength = upMove.Length();
	const float upDist = upDistConst + upMoveLength;
	const float onGroundDist = upDistConst * 2.f + upMoveLength;
	const CU::Vector3f pos = transformation.GetPosition();

	myIsOnGround = false;

	//Update fall speed per wheel

	CU::Vector3f examineVector = pos;

	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector + upMove, down, testLength, Physics::eGround);

	if (raycastHitData.hit == true)
	{
		if (raycastHitData.distance < onGroundDist)
		{
			myIsOnGround = true;
		}
		if (raycastHitData.distance < upDist)
		{
			myMainSpeed = 0;

			const float disp = upDist - raycastHitData.distance;

			examineVector -= down * (disp < 0.f ? 0.f : disp);
		}
	}


	myMainSpeed += gravity * aDeltaTime;


	//When not on ground, do fall
	const CU::Vector3f disp = down * myMainSpeed * aDeltaTime;


	examineVector += disp;
	

	GetParent()->SetWorldPosition(examineVector);

	//Set model rotation
	SComponentMessageData messageData;

	NotifyParent(eComponentMessageType::eMoving, messageData);
}
