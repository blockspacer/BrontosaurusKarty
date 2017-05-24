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
#include "KartControllerComponentManager.h"

CKartControllerComponent::CKartControllerComponent(CKartControllerComponentManager* aManager): myPhysicsScene(nullptr), myIsOnGround(true), myCanAccelerate(false), myManager(aManager)
{
	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/KartStats.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("Karts");

	CU::CJsonValue Karts = levelsArray.at("BaseKart");


	//myFowrardSpeed = 0.0f;
	myMaxSpeed = Karts.at("MaxSpeed").GetFloat();
	myMinSpeed = Karts.at("ReverseSpeed").GetFloat();
	myAcceleration = 0.0f;

	myMaxAcceleration = Karts.at("MaxAcceleration").GetFloat();
	myMinAcceleration = Karts.at("ReverseAcceleration").GetFloat();

	myTurnRate = Karts.at("TurnRate").GetFloat();

	myGrip = Karts.at("Grip").GetFloat();
	myWeight = Karts.at("Weight").GetFloat();

	mySteering = 0.f;
	myAngularAcceleration = Karts.at("AngulareAcceleration").GetFloat();
	myDriftAngle = Karts.at("DriftAngle").GetFloat();

	myMaxSpeedModifier = 1.0f;
	myAccelerationModifier = 1.0f;

	myIsBoosting = false;
	myHasGottenHit = false;
	myTimeToBeStunned = 1.5f;
	myElapsedStunTime = 0.f;


	myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;

	myDrifter = std::make_unique<CDrifter>();
	myDrifter->Init(Karts);

	myLeftWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
	myRightWheelDriftEmmiterHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("DriftParticle").GetString());
	myLeftDriftBoostEmitterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("FirstStageBoostParticle").GetString());
	myRightDriftBoostEmitterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance(Karts.at("FirstStageBoostParticle").GetString());
	myBoostEmmiterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("GunFire");

	myAirControl = Karts.at("AirControl").GetFloat();

	myCurrentAction = eCurrentAction::eDefault;
}


CKartControllerComponent::~CKartControllerComponent()
{
}

void CKartControllerComponent::Turn(float aDirectionX)
{
	if (myHasGottenHit == true)
	{
		return;
	}
	if (aDirectionX < 0.f)
	{
		TurnLeft(aDirectionX * aDirectionX);
	}
	else if (aDirectionX > 0.f)
	{
		TurnRight(aDirectionX * aDirectionX);
	}
}

void CKartControllerComponent::TurnRight(const float aNormalizedModifier)
{
	DL_PRINT("turning right: %f", aNormalizedModifier);
	if (myHasGottenHit == true)
	{
		return;
	}
	assert(aNormalizedModifier <= 1.f && aNormalizedModifier >= -1.f && "normalized modifier not normalized mvh carl");
	myCurrentAction = eCurrentAction::eTurningRight;

	if (myDrifter->IsDrifting() == false)
	{
		mySteering = myTurnRate * aNormalizedModifier;
	}
	else
	{
		myDrifter->TurnRight();
	}
}

void CKartControllerComponent::TurnLeft(const float aNormalizedModifier)
{
	DL_PRINT("turning left: %f", aNormalizedModifier);
	if (myHasGottenHit == true)
	{
		return;
	}
	myCurrentAction = eCurrentAction::eTurningLeft;
	if (myDrifter->IsDrifting() == false)
	{
		mySteering = myTurnRate * aNormalizedModifier;
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
	if (myHasGottenHit == true)
	{
		return;
	}
	myAcceleration = myMaxAcceleration;
}

void CKartControllerComponent::MoveBackWards()
{
	if (myHasGottenHit == true)
	{
		return;
	}
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
	if (myHasGottenHit == true)
	{
		return;
	}
	myDrifter->StartDrifting(mySteering);
	SComponentMessageData messageData;
	messageData.myFloat = myDriftAngle;
	if (mySteering > 0)
	{
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
		//SetVibrationOnController* vibrationMessage = new SetVibrationOnController(0, 10, 10);
		//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
	}
	else if (mySteering < 0)
	{
		messageData.myFloat *= -1.f;
		CParticleEmitterManager::GetInstance().Activate(myLeftWheelDriftEmmiterHandle);
		CParticleEmitterManager::GetInstance().Activate(myRightWheelDriftEmmiterHandle);
		//SetVibrationOnController* vibrationMessage = new SetVibrationOnController(0, 10, 10);
		//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
	}
	else
	{
		messageData.myFloat = 0.f;
	}

	//myVelocity += CU::Vector3f::UnitY * 5;
	
	GetParent()->NotifyComponents(eComponentMessageType::eDoDriftBobbing, messageData);
}

void CKartControllerComponent::StopDrifting()
{
	GetParent()->NotifyComponents(eComponentMessageType::eCancelDriftBobbing, SComponentMessageData());
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

void CKartControllerComponent::GetHit()
{
	myHasGottenHit = true;
	StopDrifting();
	//myAcceleration = 0;
}

//TODO: Hard coded, not good, change soon
const float killHeight = -50;

void CKartControllerComponent::CheckZKill()
{
	const float height = GetParent()->GetWorldPosition().y;

	if(height < killHeight)
	{
		/*GetParent()->SetWorldTransformation(CU::Matrix44f());
		GetParent()->SetWorldPosition(CU::Vector3f(0.f, 1.f, 0.f));*/
		//myFowrardSpeed = 0.f;
		myVelocity = CU::Vector3f::Zero;
		GetParent()->NotifyComponents(eComponentMessageType::eKill, SComponentMessageData());
		GetParent()->NotifyComponents(eComponentMessageType::eRespawn, SComponentMessageData());
	}
}

void CKartControllerComponent::Update(const float aDeltaTime)
{
	DoPhysics(aDeltaTime);
	CheckZKill();
	
	UpdateMovement(aDeltaTime);
	
	if (myIsBoosting == true)
	{
		CU::Matrix44f particlePosition = GetParent()->GetLocalTransform();

		particlePosition.Move(CU::Vector3f(0.f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myBoostEmmiterhandle, particlePosition.GetPosition());
	}

	if (myHasGottenHit == true)
	{
		myElapsedStunTime += aDeltaTime;
		if (myElapsedStunTime >= myTimeToBeStunned)
		{
			myElapsedStunTime = 0;
			myHasGottenHit = false;
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
	case eComponentMessageType::eOnCollisionEnter:
		{
		int i = 0;
		}
		break;
	case eComponentMessageType::eAddComponent:
		break;
	case eComponentMessageType::eObjectDone:
		break;
	case eComponentMessageType::eSetBoost:

		if (aMessageData.myBoostData->maxSpeedBoost > 0)
		{
			myIsBoosting = true;
			CParticleEmitterManager::GetInstance().Activate(myBoostEmmiterhandle);
		}
		else
		{
			myIsBoosting = false;
			CParticleEmitterManager::GetInstance().Deactivate(myBoostEmmiterhandle);
		}

		myMaxSpeedModifier = 1.0f + aMessageData.myBoostData->maxSpeedBoost;
		myAccelerationModifier = 1.0f + aMessageData.myBoostData->accerationBoost;
		myBoostSpeedDecay = myMaxAcceleration * myAccelerationModifier * 1.25f;
		break;
	}
}

void CKartControllerComponent::Init(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

void CKartControllerComponent::UpdateMovement(const float aDeltaTime)
{
	//Position
	const CU::Vector3f forwardVector = GetParent()->GetToWorldTransform().myForwardVector;
	const float speed = forwardVector.Dot(myVelocity);
	const float dir = myVelocity.Dot(forwardVector);

	float way = 1.f;
	if (dir > 0.f)
	{
		way = -1.f;
	}

	const float onGroundModifier = (myCanAccelerate == true ? 1.f : 0.f);
	myVelocity -= (1.25f - abs(myVelocity.GetNormalized().Dot(forwardVector))) *
		myVelocity * (myGrip / myWeight)  * aDeltaTime * onGroundModifier;

	if (myHasGottenHit == false)
	{
		myVelocity += forwardVector * aDeltaTime * myAcceleration * myGrip * myAccelerationModifier * onGroundModifier;
	}

	const float maxSpeed2 = myMaxSpeed * myMaxSpeed * myMaxSpeedModifier * myMaxSpeedModifier;
	const float minSpeed2 = myMinSpeed * myMinSpeed;
	const float speed2 = myVelocity.Length2() * (dir > 0.f ? 1.f : -1.f);
	if (dir > 0.f && speed2 > maxSpeed2)
	{
		float speed = myVelocity.Length();
		myVelocity.Normalize();

		speed -= myBoostSpeedDecay * aDeltaTime;
		myVelocity *= speed;
	}
	if (dir < 0.f && speed2 < minSpeed2)
	{
		myVelocity.Normalize();
		myVelocity *= -myMinSpeed;

	}

	if (myDrifter->IsDrifting() == true)
	{
		DoDriftingParticles();

		myDrifter->ApplySteering(mySteering, aDeltaTime);
		//GetParent()->SetWorldPosition(GetParent()->GetWorldPosition() + myVelocity * aDeltaTime);
	}
	//else
	//{
		GetParent()->Move(CU::Vector3f::UnitZ * speed * aDeltaTime);
	//}
	GetParent()->Move(CU::Vector3f::UnitY * myVelocity.y * aDeltaTime);

	//Steering
	float steerAngle = (mySteering + myDrifter->GetSteerModifier()) * myAngularAcceleration * -way * (myIsOnGround == true ? 1.f : myAirControl);
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	parentTransform.RotateAroundAxis(steerAngle * (abs(speed2) < 0.001f ? 0.f : 1.f) * aDeltaTime, CU::Axees::Y);
}

void CKartControllerComponent::DoDriftingParticles()
{
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
	const float controlDist = upDistConst * 100.f + upMoveLength;
	const CU::Vector3f pos = transformation.GetPosition();

	myIsOnGround = false;
	myCanAccelerate = false;

	//Update fall speed per wheel

	CU::Vector3f examineVector = pos;

	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector + upMove, down, testLength, Physics::eGround);

	CU::Vector3f downAccl = down;
	float slopeModifier = 1.f;
	float friction = 1.f;
	if (raycastHitData.hit == true)
	{
		const CU::Vector3f& norm = raycastHitData.normal;
		if(raycastHitData.distance < controlDist)
		{
			myIsOnGround = true;
		}
		if (raycastHitData.distance < onGroundDist)
		{

			downAccl = norm.Cross(down.Cross(norm));
			friction = norm.Dot(-down);
			myCanAccelerate = true;
			
		}
		if (raycastHitData.distance < upDist)
		{
			const float speed = myVelocity.Length();
			//myVelocity.y = 0.f; downAccl * speed;
			
			const float disp = upDist - raycastHitData.distance;

			GetParent()->GetLocalTransform().Move(norm * (disp < 0.f ? 0.f : disp));
		}
	}


	myVelocity += downAccl * (friction / (myCanAccelerate == true ? myGrip / myWeight : 1.f)) *gravity * aDeltaTime;
}

bool CKartControllerComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eGetIsGrounded:
		if(myIsOnGround == true)
		{
			return true;
		}
		break;
	case eComponentQuestionType::eGetSplineDirection:
		aQuestionData.myVector3f = myManager->GetClosestSpinesDirection(GetParent()->GetWorldPosition());
		return true;
		break;
	case eComponentQuestionType::eGetSplineWithIndex:
		aQuestionData.myNavigationPoint = myManager->GetNavigationPoint(aQuestionData.myInt);
		return true;
		break;
	default:
		break;
	}
	return false;
}