#include "stdafx.h"
#include "KartControllerComponent.h"
#include "SpeedHandlerManager.h"
#include "BoostData.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"
#include "Drifter.h"
#include "KartAnimator.h"
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
#include "ColliderComponent.h"

#include "../Audio/AudioInterface.h"
#include "../BrontosaurusEngine/Scene.h"
#include "../BrontosaurusEngine/DecalInstance.h"


CKartControllerComponent::CKartControllerComponent(CKartControllerComponentManager* aManager, CModelComponent& aModelComponent, const short aControllerIndex, const short aCharacterIndex)
	: myPhysicsScene(nullptr)
	, myIsOnGround(true)
	, myCanAccelerate(false)
	, myManager(aManager)
	, myTerrainModifier(1.f)
	, myIsOnGroundLast(false)
	, myLastGroundComponent(nullptr)
	, myLookingBack(false)
{
	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/KartStats.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());
	CU::CJsonValue Karts = levelsFile.at("Karts")[aCharacterIndex];

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
	myTargetSteering = 0.f;
	myAngularAcceleration = Karts.at("AngulareAcceleration").GetFloat();
	myDriftAngle = Karts.at("DriftAngle").GetFloat();
	if(Karts.HasKey("TerrainResistance") == true)
	{
		myTerrainResistance = 1.f - Karts.at("TerrainResistance").GetFloat();
	}
	else
	{
		myTerrainResistance = 1.f;
		DL_PRINT("Kart does not define \"TerrainResistance\".");
	}

	myMaxSpeedModifier = 1.0f;
	myAccelerationModifier = 1.0f;

	myIsBoosting = false;
	myIsInvurnable = false;
	myInvurnableTime = 0;
	myElapsedInvurnableTime = 0;

	myHasGottenHit = false;
	myIsAIControlled = false;
	myIsplayingEngineLoop = false;
	myPreviousGotHit = false;

	myTimeToBeStunned = 1.5f;
	myElapsedStunTime = 0.f;
	myModifierCopy = 0.0f;

	myDriftSetupTimer = 0.3f;
	myDriftSetupTime = 0.3f;

	myControllerHandle = aControllerIndex;

	myBoostSpeedDecay = GetMaxAcceleration() * myAccelerationModifier * 1.25f;

	myDrifter = std::make_unique<CDrifter>();
	myDrifter->Init(Karts);
	myAnimator = std::make_unique<CKartAnimator>(aModelComponent, Karts);

	myBoostEmmiterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("GunFire");
	myGotHitEmmiterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("Stars");
	myStarEmmiterhandle1 = CParticleEmitterManager::GetInstance().GetEmitterInstance("StarBoost");
	myStarEmmiterhandle2 = CParticleEmitterManager::GetInstance().GetEmitterInstance("StarBoost");
	mySlowMovment = CParticleEmitterManager::GetInstance().GetEmitterInstance("SlowSmoke");
	myGrassEmmiter1 = CParticleEmitterManager::GetInstance().GetEmitterInstance("Grass");
	myGrassEmmiter2 = CParticleEmitterManager::GetInstance().GetEmitterInstance("Grass");

	myPreRaceBoostRate = 0.0f;
	myPreRaceBoostValue = 0.0f;
	myPreRaceRate = Karts.at("CountDownBoostRate").GetFloat();
	myPreRaceBoostTarget = Karts.at("CountDownBoostTarget").GetFloat();

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
		TurnLeft(aDirectionX * std::fabs(aDirectionX));
	}
	else if (aDirectionX > 0.f)
	{
		TurnRight(aDirectionX * std::fabs(aDirectionX));
	}
}

const float rate = 5.f;
void CKartControllerComponent::TurnRight(const float aNormalizedModifier)
{
	myCurrentAction = eCurrentAction::eTurningRight;
	if (myHasGottenHit == true)
	{
		return;
	}
	assert(aNormalizedModifier <= 1.f && aNormalizedModifier >= -1.f && "normalized modifier not normalized mvh carl");
	if (myDriftSetupTimer < myDriftSetupTime)
	{
		if (myDrifter->IsDrifting() == false)
		{
			Drift();
			myDrifter->TurnRight();
		}
	}
	myModifierCopy = aNormalizedModifier;
	if (myDrifter->IsDrifting() == false)
	{
		float maxSteering = myTurnRate * aNormalizedModifier;
		myTargetSteering = maxSteering;
	}
	else
	{
		myDrifter->TurnRight();
	}

	//if (mySteering <= 0.f)
	{
		myAnimator->OnTurnRight(aNormalizedModifier);
	}
}

void CKartControllerComponent::TurnLeft(const float aNormalizedModifier)
{
	myCurrentAction = eCurrentAction::eTurningLeft;
	if (myHasGottenHit == true)
	{
		return;
	}
	if (myDriftSetupTimer < myDriftSetupTime)
	{
		if (myDrifter->IsDrifting() == false)
		{
			Drift();
			myDrifter->TurnLeft();
		}
	}
	if (myDrifter->IsDrifting() == false)
	{
		float maxSteering = myTurnRate * aNormalizedModifier;
		myTargetSteering = maxSteering;
	}
	else
	{
		myDrifter->TurnLeft();
	}
	myAnimator->OnTurnLeft(aNormalizedModifier);
}

void CKartControllerComponent::StopMoving()
{
	myAcceleration = 0;
	myAnimator->OnStopMoving();
	myIsHoldingForward = false;
}

void CKartControllerComponent::MoveFoward()
{
	myIsHoldingForward = true;
	if (myHasGottenHit == true)
	{
		return;
	}
	myAcceleration = GetMaxAcceleration();
	myAnimator->OnMoveFoward();
}

void CKartControllerComponent::MoveForwardWithoutChangingHoldingForward()
{
	if (myHasGottenHit == true)
	{
		return;
	}
	myAcceleration = GetMaxAcceleration();
	myAnimator->OnMoveFoward();
}

void CKartControllerComponent::MoveBackWards()
{
	if (myHasGottenHit == true)
	{
		return;
	}
	myAcceleration = myMinAcceleration;
	myAnimator->OnMoveBackWards(myVelocity.z);
	myIsHoldingForward = false;
}

void CKartControllerComponent::StopTurning()
{
	if (myCurrentAction == eCurrentAction::eTurningLeft)
	{
		myAnimator->OnStopTurningLeft();
	}
	else if (myCurrentAction == eCurrentAction::eTurningRight)
	{
		myAnimator->OnStopTurningRight();
	}
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
bool CKartControllerComponent::Drift()
{
	if (myIsOnGround == false)
	{
		return false;
	}
	if (myHasGottenHit == true)
	{
		return false;
	}
	if (myDrifter->IsDrifting() == true)
	{
		return false;
	}
	if (myVelocity.Length() < myMaxSpeed * 0.5f)
	{
		return false;
	}

	myDrifter->StartDrifting(myCurrentAction);
	myAnimator->OnDrift();
	
	SComponentMessageData messageData;
	messageData.myFloat = myDriftAngle;
	if (myCurrentAction == eCurrentAction::eDefault)
	{
		messageData.myFloat = 0.f;
		myDriftSetupTimer = 0.0f;
	}
	else
	{
		if (myCurrentAction == eCurrentAction::eTurningLeft)
		{
			messageData.myFloat *= -1.f;
		}
		if (myControllerHandle != -1 && myControllerHandle < 4)
		{
			SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerHandle, 20, 20);
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
		}
		GetParent()->NotifyComponents(eComponentMessageType::eDoDriftBobbing, messageData);
	}

	SComponentMessageData data;
	data.myString = "PlayDrift";
	GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);

	return true;
}

void CKartControllerComponent::StopDrifting(const bool aShouldGetBoost)
{
	myDriftSetupTimer = myDriftSetupTime + 1.0f;
	GetParent()->NotifyComponents(eComponentMessageType::eCancelDriftBobbing, SComponentMessageData());
	CDrifter::eDriftBoost boost = myDrifter->StopDrifting();

	SComponentMessageData data;
	data.myString = "StopDrift";
	GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);

	myAnimator->OnStopDrifting();

	if (myControllerHandle != -1 && myControllerHandle < 4)
	{
		StopVibrationOnController* stopionMessageLeft = new StopVibrationOnController(myControllerHandle);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(stopionMessageLeft);
	}
	if (aShouldGetBoost == true)
	{
		SComponentMessageData boostMessageData;
		switch (boost)
		{
		case CDrifter::eDriftBoost::eLarge:
			boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("DriftBoost"));
			GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
			if (myControllerHandle != -1 && myControllerHandle < 4)
			{
				SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerHandle, 40, 60, 0.8f, false);
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
			}
			break;
		case  CDrifter::eDriftBoost::eSmall:
			boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("MiniDriftBoost"));
			GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
			if (myControllerHandle != -1 && myControllerHandle < 4)
			{
				SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerHandle, 25, 50, 0.5f, false);
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
			}
			break;
		case CDrifter::eDriftBoost::eNone:
			break;
		}
	}


	switch (myCurrentAction)
	{
	case eCurrentAction::eTurningRight:
		TurnRight();
		break;
	case eCurrentAction::eTurningLeft:
		TurnLeft();
		break;
	case eCurrentAction::eDefault:
		StopTurning();
		break;
	default:
		break;
	}
}

void CKartControllerComponent::GetHit()
{
	if (myIsInvurnable == false && myHasGottenHit == false)
	{
		if (myControllerHandle != -1 && myControllerHandle < 4)
		{
			SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerHandle, 70, 30, 0.5f, false);
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
		}
		myHasGottenHit = true;
		StopDrifting(false);
		GetParent()->NotifyComponents(eComponentMessageType::eSpinKart, SComponentMessageData());
		CParticleEmitterManager::GetInstance().Activate(myGotHitEmmiterhandle);
		SComponentMessageData sound; sound.myString = "PlayGetHit";
		GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
	}
	//myAcceleration = 0;
}

void CKartControllerComponent::ApplyStartBoost()
{
	float smallboost = myPreRaceBoostTarget * 0.6f;
	float bigboost = myPreRaceBoostTarget * 0.85f;
	if (myPreRaceBoostValue > myPreRaceBoostTarget * 0.6f)
	{
		myAnimator->OnStartBoosting();
		if (myPreRaceBoostValue > myPreRaceBoostTarget * 0.8f)
		{
			if (myPreRaceBoostValue > myPreRaceBoostTarget)
			{
				GetHit();
				return;
			}
			SComponentMessageData boostMessageData;
			boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("DriftBoost"));
			GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
			if (myControllerHandle != -1 && myControllerHandle < 4)
			{
				SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerHandle, 40, 60, 0.8f, false);
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
			}
			return;
		}
		SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("MiniDriftBoost"));
		GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		if (myControllerHandle != -1 && myControllerHandle < 4)
		{
			SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerHandle, 25, 50, 0.5f, false);
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
		}
		return;
	}
}

void CKartControllerComponent::IncreasePreGameBoostValue()
{
	myPreRaceBoostRate = myPreRaceRate;
}

void CKartControllerComponent::DecreasePreGameBoostValue()
{
	myPreRaceBoostRate = -myPreRaceRate * 0.6f;
}

//TODO: Hard coded, not good, change soon
const float killHeight = -50;

void CKartControllerComponent::ZKill()
{
	myVelocity = CU::Vector3f::Zero;
	GetParent()->NotifyComponents(eComponentMessageType::eKill, SComponentMessageData());
	GetParent()->NotifyComponents(eComponentMessageType::eRespawn, SComponentMessageData());
	
}

void CKartControllerComponent::Update(const float aDeltaTime)
{
	SComponentMessageData messageData;

	myIsOnGround = true;
	myCanAccelerate = true;
	DoPhysics(aDeltaTime);
	CheckWallKartCollision(aDeltaTime);

	messageData.myFloat = aDeltaTime;
	GetParent()->NotifyComponents(eComponentMessageType::eUpdate, messageData);
	myDriftSetupTimer += aDeltaTime;
	UpdateMovement(aDeltaTime);

	if (myIsBoosting == true)
	{
		CU::Matrix44f particlePosition = GetParent()->GetLocalTransform();

		particlePosition.Move(CU::Vector3f(0.f, 0, 0));
		CParticleEmitterManager::GetInstance().SetPosition(myBoostEmmiterhandle, particlePosition.GetPosition());
	}

	if (myMaxSpeedModifier < 1)
	{
		//start to emmit
		CParticleEmitterManager::GetInstance().Activate(mySlowMovment);

		CU::Matrix44f transform = GetParent()->GetToWorldTransform();

		transform.Move(CU::Vector3f(0, 0.5f, 0));

		CParticleEmitterManager::GetInstance().SetPosition(mySlowMovment, transform.GetPosition());
	}
	else
	{
		CParticleEmitterManager::GetInstance().Deactivate(mySlowMovment);
	}

	if (myVelocity.Length() > 1.0f)
	{
		if (myIsplayingEngineLoop == false)
		{
			SComponentMessageData data; data.myString = "PlayEngineLoop";
			GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);
			myIsplayingEngineLoop = true;
		}
	}
	else
	{
		SComponentMessageData data; data.myString = "StopEngineLoop";
		GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);
		myIsplayingEngineLoop = false;
	}

	if (myTerrainModifier < 1.0f && myVelocity.Length() > 1.0f)
	{
		//start grass particles
		CParticleEmitterManager::GetInstance().Activate(myGrassEmmiter1);
		CParticleEmitterManager::GetInstance().Activate(myGrassEmmiter2);

		CU::Matrix44f transform = GetParent()->GetToWorldTransform();

		transform.Move(CU::Vector3f(-0.45f, 0, 0));

		CParticleEmitterManager::GetInstance().SetPosition(myGrassEmmiter1, transform.GetPosition());

		transform = GetParent()->GetToWorldTransform();

		transform.Move(CU::Vector3f(0.5f, 0, 0));

		CParticleEmitterManager::GetInstance().SetPosition(myGrassEmmiter2, transform.GetPosition());
	}
	else
	{
		//stop grass partickles
		CParticleEmitterManager::GetInstance().Deactivate(myGrassEmmiter1);
		CParticleEmitterManager::GetInstance().Deactivate(myGrassEmmiter2);
	}

	if (myHasGottenHit == true)
	{
		myElapsedStunTime += aDeltaTime;
		CParticleEmitterManager::GetInstance().SetPosition(myGotHitEmmiterhandle, GetParent()->GetWorldPosition());
		if (myElapsedStunTime >= myTimeToBeStunned)
		{
			myElapsedStunTime = 0;
			myHasGottenHit = false;
			CParticleEmitterManager::GetInstance().Deactivate(myGotHitEmmiterhandle);
		}
	}

	if (myIsInvurnable == true)
	{
		myElapsedInvurnableTime += aDeltaTime;

		CU::Matrix44f transform = GetParent()->GetToWorldTransform();

		transform.Move(CU::Vector3f(0.5f, 0, 0));

		CParticleEmitterManager::GetInstance().SetPosition(myStarEmmiterhandle1, transform.GetPosition());

		transform = GetParent()->GetToWorldTransform();

		transform.Move(CU::Vector3f(-0.5f, 0, 0));

		CParticleEmitterManager::GetInstance().SetPosition(myStarEmmiterhandle2, transform.GetPosition());



		if (myElapsedInvurnableTime >= myInvurnableTime)
		{
			myElapsedInvurnableTime = 0;
			myIsInvurnable = false;
			SComponentMessageData sound; sound.myString = "StopStar";
			GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eTurnOffHazard, SComponentMessageData());
			CParticleEmitterManager::GetInstance().Deactivate(myStarEmmiterhandle1);
			CParticleEmitterManager::GetInstance().Deactivate(myStarEmmiterhandle2);
		}
	}

	GetParent()->NotifyComponents(eComponentMessageType::eMoving, messageData);

	myAnimator->Update(aDeltaTime, myVelocity.z, mySteering);
}

void CKartControllerComponent::CountDownUpdate(const float aDeltaTime)
{
	myPreRaceBoostValue += myPreRaceBoostRate * aDeltaTime;
	if (myPreRaceBoostValue < 0.0f)
	{
		myPreRaceBoostValue = 0.0f;
	}
}

const CNavigationSpline & CKartControllerComponent::GetNavigationSpline()
{
	return myManager->GetNavigationSpline();
}

void CKartControllerComponent::LookBack(bool aLookBack)
{
	if(myLookingBack != aLookBack)
	{
		myLookingBack = aLookBack;;

		SComponentMessageData data;

		CU::Matrix33f rotation;
		data.myVoidPointer = &rotation;
		if(myLookingBack == true)
		{
			rotation *= CU::Matrix33f::CreateRotateAroundY(TAU / 2.f);
		}
		GetParent()->NotifyComponents(eComponentMessageType::eSetCameraRotation, data);
	}
}



void CKartControllerComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		break;
	case eComponentMessageType::eObjectDone:
		break;
	case eComponentMessageType::eGotHit:
	{
		GetHit();
		break;
	}
	case eComponentMessageType::eMakeInvurnable:
	{
		myInvurnableTime = aMessageData.myBoostData->duration;
		myElapsedInvurnableTime = 0;
		CParticleEmitterManager::GetInstance().Activate(myStarEmmiterhandle1);
		CParticleEmitterManager::GetInstance().Activate(myStarEmmiterhandle2);
		if (myIsInvurnable == false)
		{
			SComponentMessageData sound; sound.myString = "PlayStar";
			GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
		}
		myIsInvurnable = true;
		break;
	}
	case eComponentMessageType::eSetBoost:
	{
		if (aMessageData.myBoostData->maxSpeedBoost > 0)
		{
			myIsBoosting = true;
			CParticleEmitterManager::GetInstance().Activate(myBoostEmmiterhandle);
		}
		else
		{
			myIsBoosting = false;
			if (myIsHoldingForward == false)
			{
				StopMoving();
			}
			else
			{
				MoveFoward();
			}
			CParticleEmitterManager::GetInstance().Deactivate(myBoostEmmiterhandle);
		}

		myMaxSpeedModifier = 1.0f + aMessageData.myBoostData->maxSpeedBoost;
		myAccelerationModifier = 1.0f + aMessageData.myBoostData->accerationBoost;
		myBoostSpeedDecay = GetMaxAcceleration() * myAccelerationModifier * 1.25f;
		break;
	}
	case eComponentMessageType::eAITakeOver:
	{
		myIsAIControlled = true;
	}
	}
}

void CKartControllerComponent::Init(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

bool CKartControllerComponent::IsFutureGrounded(const float aDistance)
{
	CU::Vector3f orig = GetParent()->GetWorldPosition();
	orig.y += 50.f;
	CU::Vector3f down(0.0f, -1.0f, 0.0f);
	orig += GetParent()->GetToWorldTransform().myForwardVector * aDistance;

	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(orig, down, 100.0f, Physics::eGround);
	return raycastHitData.hit;
}

const CU::Vector3f& CKartControllerComponent::GetVelocity() const
{
	return myVelocity;
}

void CKartControllerComponent::UpdateMovement(const float aDeltaTime)
{
	if (GetHitGround() == true)
	{
		if (myControllerHandle != -1 && myControllerHandle < 4)
		{
			SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerHandle, 35, 70, 0.15f, false);
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
		}
	}
	//Position
	if (myHasGottenHit == false)
	{
		if (myHasGottenHit != myPreviousGotHit)
		{
			myHasGottenHit = myPreviousGotHit;
			switch (myCurrentAction)
			{
			case eCurrentAction::eTurningRight:
				TurnRight();
				break;
			case eCurrentAction::eTurningLeft:
				TurnLeft();
				break;
			case eCurrentAction::eDefault:
				break;
			default:
				break;
			}
			if (myIsHoldingForward == true)
			{
				MoveBackWards();
			}
		}
	}

	CU::Vector3f forwardVector(CU::Vector3f::UnitZ);

	if (myIsOnGround == true)
	{
		const CU::Vector3f localSpaceNormal = myGroundNormal * GetParent()->GetToWorldTransform().GetRotation().GetInverted();
		const CU::Vector3f localSpceNormalXForward = localSpaceNormal.Cross(CU::Vector3f::UnitZ);
		forwardVector = localSpceNormalXForward.Cross(localSpaceNormal);
	}

	const float speed = forwardVector.Dot(myVelocity);
	const float dir = myVelocity.Dot(forwardVector);

	float way = 1.f;
	if (dir > 0.f)
	{
		way = -1.f;
	}

	const float onGroundModifier = (myCanAccelerate == true ? 1.f : 0.f);
	const float gripOverWeight = (myGrip / myWeight);
	myVelocity -= myVelocity * CU::Vector3f(40.f, 1.f, 1.f) * myGrip * aDeltaTime * onGroundModifier;

	if (myHasGottenHit == false)
	{
		myVelocity += forwardVector * aDeltaTime * GetAcceleratiot() * myGrip * myAccelerationModifier * onGroundModifier;
	}
	if (myIsBoosting == true)
	{
		MoveForwardWithoutChangingHoldingForward();
	}
	

	float y =myVelocity.y;
	const float maxSpeed2 = GetMaxSpeed2() * myMaxSpeedModifier * myMaxSpeedModifier;
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
	if (GetIsGrounded() == false)
	{
		myVelocity.y = y;
	}
	float steerAngle = 0.f;

	GetParent()->Move(myVelocity* myDrifter->GetDriftBonusSpeed() *aDeltaTime);

	if (myDrifter->IsDrifting() == true)
	{
		if (myVelocity.Length() < myMaxSpeed * 0.5f)
		{
			StopDrifting(false);
		}
		else
		{
			myDrifter->UpdateDriftParticles(GetParent()->GetLocalTransform());
			myDrifter->GetSteering(steerAngle, myTurnRate, myAngularAcceleration, way, myAirControl, myIsOnGround, myCurrentAction, aDeltaTime);
		}
	}
	else
	{
		switch (myCurrentAction)
		{
		case eCurrentAction::eTurningRight:
			if (mySteering < myTargetSteering)
			{
				mySteering += myTurnRate * rate * aDeltaTime;
				if (mySteering > myTargetSteering)
				{
					mySteering = myTargetSteering;
				}
			}
			break;
		case eCurrentAction::eTurningLeft:
			if (mySteering > myTargetSteering)
			{
				mySteering -= myTurnRate * rate * aDeltaTime;
				if (mySteering < myTargetSteering)
				{
					mySteering = myTargetSteering;
				}
			}
			break;
		case eCurrentAction::eDefault:
			break;
		default:
			break;
		}
		steerAngle = (mySteering + myDrifter->GetSteerModifier()) * myAngularAcceleration * -way * (myIsOnGround == true ? 1.f : myAirControl);
	}

	//Steering
	CU::Matrix44f& parentTransform = GetParent()->GetLocalTransform();
	parentTransform.RotateAroundAxis(steerAngle * (abs(speed2) < 0.001f ? 0.f : 1.f) * aDeltaTime, CU::Axees::Y);
}

static const unsigned int numberOfCorners = 4;
static const unsigned int testsPerCorner = 2;

void CKartControllerComponent::DoCornerTest(unsigned aCornerIndex, const CU::Matrix33f& aRotationMatrix, const CU::Vector3f& aPosition, const float aHalfWidth, const float aLength)
{
	const bool right = aCornerIndex < 2;
	const bool top = aCornerIndex % 2 == 0;

	const CU::Vector3f cornerPos = aPosition + CU::Vector3f(
		(right ? aHalfWidth : -aHalfWidth),
		0.f,
		(top ? aLength : 0.f)
	) * aRotationMatrix;

	for (unsigned i = 0; i < testsPerCorner; ++i)
	{
		const CU::Vector3f testDir = (i % 2 == 0 ? (
			right ? aRotationMatrix.myRightVector : -aRotationMatrix.myRightVector) :
			(top ? aRotationMatrix.myForwardVector : -aRotationMatrix.myForwardVector));

		static const float testDist = 0.25f;
		Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(cornerPos, testDir, testDist,
			static_cast<Physics::ECollisionLayer>(Physics::eWall | Physics::eKart));


		if (raycastHitData.hit == true)
		{
			static const float bounceEffect = 2.5f;
			const CU::Matrix33f rot = GetParent()->GetToWorldTransform().GetRotation().GetInverted();
			const CU::Vector3f pos = aPosition;
			const CU::Vector3f dir = raycastHitData.normal * rot;//(pos - raycastHitData.position).Normalize() * rot;
			if (raycastHitData.collisionLayer == Physics::eWall)
			{
				GetParent()->Move(dir * (raycastHitData.distance - testDist) * -2.f);
				myVelocity *= 0.75f;
				const float repulsion = CLAMP(myVelocity.Length() * bounceEffect, 0.f, GetMaxSpeed() * 2.f);
				myVelocity += repulsion * dir;
			}
			else if(raycastHitData.collisionLayer == Physics::eKart && reinterpret_cast<CComponent*>(raycastHitData.actor->GetCallbackData()->GetUserData())->GetParent() != GetParent())
			{
				if (myIsInvurnable == true)
				{
					reinterpret_cast<CComponent*>(raycastHitData.actor->GetCallbackData()->GetUserData())->GetParent()->NotifyOnlyComponents(eComponentMessageType::eGotHit, SComponentMessageData());
				}
				GetParent()->Move(dir * (raycastHitData.distance - testDist) * -2.f);
				myVelocity *= 0.75f;
				const float repulsion = CLAMP(bounceEffect, 0.f, GetMaxSpeed() * 2.f);
				myVelocity += repulsion * dir;
			}
		}
	}
}

void CKartControllerComponent::CheckWallKartCollision(const float aDetltaTime)
{
	const CU::Matrix33f localSpace = GetParent()->GetToWorldTransform();
	const CU::Vector3f globalPos = GetParent()->GetWorldPosition();
	const float halfWidth = myAxisDescription.width / 2.f;
	const float length = myAxisDescription.length;

	for (unsigned i = 0; i < numberOfCorners; ++i)
	{
		DoCornerTest(i, localSpace, globalPos, halfWidth, length);
	}
}

const float gravity = 9.82f * 2.f;
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

	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector + upMove, down, testLength, 
		static_cast<Physics::ECollisionLayer>(Physics::eGround | Physics::eKill));

	CU::Vector3f downAccl = down;
	float friction = 1.f;
	if (raycastHitData.hit == true)
	{
		if(raycastHitData.collisionLayer == Physics::eGround)
		{
			const CU::Vector3f& norm = raycastHitData.normal;
			myGroundNormal = norm;
			if (raycastHitData.distance < controlDist)
			{
				myIsOnGround = true;
			}
			if (raycastHitData.distance < onGroundDist)
			{
				friction = 0.f;
				myCanAccelerate = true;

				CComponent* component = reinterpret_cast<CComponent*>(raycastHitData.actor->GetCallbackData()->GetUserData())->GetParent();

				if (component != myLastGroundComponent)
				{
					myLastGroundComponent = component;
					SComponentQuestionData questionData;
					if (myLastGroundComponent->Answer(eComponentQuestionType::eGetTerrainModifier, questionData))
					{
						myTerrainModifier = questionData.myFloat;
					}
					else
					{
						myTerrainModifier = 1.f;
					}
				}

			}
			if (raycastHitData.distance < upDist)
			{
				const float speed = myVelocity.Length();
				//myVelocity.y = 0.f; downAccl * speed;

				const float disp = upDist - raycastHitData.distance;

				GetParent()->GetLocalTransform().Move(norm * (disp < 0.f ? 0.f : disp));
			}
		}
		else if(raycastHitData.collisionLayer == Physics::eKill)
		{
			if (raycastHitData.distance < onGroundDist)
			{
				ZKill();
			}
		}
		
	}
	else
	{
		myTerrainModifier = 1.f;
		myLastGroundComponent = nullptr;
	}

	


	if (myCanAccelerate == false)
	{
		myVelocity += downAccl * (friction / (myCanAccelerate == true ? myGrip / myWeight : 1.f)) *gravity * aDeltaTime;
	}
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
	case eComponentQuestionType::eGetRespawnSplineWithIndex:
		aQuestionData.myNavigationPoint = myManager->GetNavigationPoint(aQuestionData.myInt);
		if(aQuestionData.myNavigationPoint == nullptr)
		{
			if(aQuestionData.myInt - 1 == -1)
			{
				aQuestionData.myNavigationPoint = myManager->GetNavigationPoint(0);
			}
			else
			{
				aQuestionData.myNavigationPoint = myManager->GetNavigationPoint(aQuestionData.myInt - 1);
			}
		}
		return true;
		break;
	case eComponentQuestionType::eCheckIfGroundAbove:
	{
		const CU::Vector3f upMove = CU::Vector3f::UnitY;
		const CU::Vector3f position = GetParent()->GetToWorldTransform().GetPosition();
		Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(position + upMove, upMove, 100.0f, Physics::eGround);
		if (raycastHitData.hit == true)
		{
			aQuestionData.myVector3f = raycastHitData.position;
			return true;
		}
		break;
	}
	default:
		break;
	}
	return false;
}

