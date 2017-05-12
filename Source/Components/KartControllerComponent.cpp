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
#include "Drifter.h"


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
		SetVibrationOnController* vibrationMessage = new SetVibrationOnController(0, 10, 10);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
	}
	else if (mySteering < 0)
	{
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

	float steerAngle = (mySteering + /*myDrifting.myDriftSteerModifier*/myDrifter->GetSteerModifier()) * myAngularAcceleration * -way;
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

	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CKartControllerComponent::ClearHeight()
{
	float height = 0.f;
	if(GetParent() != nullptr)
	{
		height = GetParent()->GetWorldPosition().y;
	}

	for(int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		SetHeight(i, height,1.f);
		SetHeight(i, height,1.f);
	}
}

void CKartControllerComponent::SetHeight(int aWheelIndex, float aHeight, const float aDt)
{
	myPreviousHeight[aWheelIndex] = myCurrentHeight[aWheelIndex];
	myCurrentHeight[aWheelIndex] = aHeight / aDt;
}

float CKartControllerComponent::GetHeightSpeed(int anIndex)
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

void CKartControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponent == this)
		{
			ClearHeight();
		}
		break;
	case eComponentMessageType::eObjectDone:
		if (myFirstMovingPass == true)
		{
			myFirstMovingPass = false;
			ClearHeight();
		}
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

void CKartControllerComponent::ClearSpeed()
{
	for (int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{
		myAxisSpeed[i] = 0.f;
	}
}

const float gravity = 9.82f;
const float upDist = 0.05f;
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

	for(int i = 0; i < static_cast<int>(AxisPos::Size); ++i)
	{

		//Update fall speed
		myAxisSpeed[i] += gravity * aDeltaTime;
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

		Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector, down, 1.f);
		if (raycastHitData.hit == true && raycastHitData.distance < upDist)
		{
			myAxisSpeed[i] = 0;
			const float disp = upDist - raycastHitData.distance;

			examineVector -= down * disp;

		}


		const float heightSpeed = GetHeightSpeed(i);

		//When not on ground, do fall
		const CU::Vector3f disp = down * myAxisSpeed[i] * aDeltaTime/* + CU::Vector3f::UnitY * heightSpeed * aDeltaTime*/;

		if(heightSpeed > 0.f)
		{
			int i = 0; 
		}
		examineVector += disp;

		axees[i] = examineVector;

		SetHeight(i, examineVector.y, aDeltaTime);
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
	
	

	transform.SetRotation(newRotation);

	transform.SetPosition(avgPos);
	GetParent()->SetWorldTransformation(transform);
	NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
}
