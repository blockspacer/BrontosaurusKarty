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

#include "../CommonUtilities/CommonUtilities.h"
CKartControllerComponent::CKartControllerComponent(CKartControllerComponentManager* aManager, CModelComponent& aModelComponent, const short aControllerIndex)
	: myPhysicsScene(nullptr)
	, myIsOnGround(true)
	, myCanAccelerate(false)
	, myManager(aManager)
	, myTerrainModifier(1.f)
	, myIsOnGroundLast(false)
	, myLastGroundComponent(nullptr)
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
	myTargetSteering = 0.f;
	myAngularAcceleration = Karts.at("AngulareAcceleration").GetFloat();
	myDriftAngle = Karts.at("DriftAngle").GetFloat();

	myMaxSpeedModifier = 1.0f;
	myAccelerationModifier = 1.0f;

	myIsBoosting = false;
	myIsInvurnable = false;
	myInvurnableTime = 0;
	myElapsedInvurnableTime = 0;

	myHasJumped = false;

	myHasGottenHit = false;
	myTimeToBeStunned = 1.5f;
	myElapsedStunTime = 0.f;
	myModifierCopy = 0.0f;

	myDriftSetupTimer = 0.3f;
	myDriftSetupTime = 0.3f;

	myControllerHandle = aControllerIndex;

	myBoostSpeedDecay = GetMaxAcceleration() * myAccelerationModifier * 1.25f;

	myDrifter = std::make_unique<CDrifter>();
	myDrifter->Init(Karts);
	myAnimator = std::make_unique<CKartAnimator>(aModelComponent);
	myBoostEmmiterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("GunFire");
	myGotHitEmmiterhandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("Stars");

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

const float rate = 3.2f;
void CKartControllerComponent::TurnRight(const float aNormalizedModifier)
{
	if (myHasGottenHit == true)
	{
		return;
	}
	assert(aNormalizedModifier <= 1.f && aNormalizedModifier >= -1.f && "normalized modifier not normalized mvh carl");
	myCurrentAction = eCurrentAction::eTurningRight;
	if (myDriftSetupTimer < myDriftSetupTime)
	{
		if (myDrifter->IsDrifting() == false)
		{
			Drift();
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

	if (mySteering <= 0.f)
	{
		myAnimator->OnTurnRight(aNormalizedModifier);
	}
}

void CKartControllerComponent::TurnLeft(const float aNormalizedModifier)
{
	if (myHasGottenHit == true)
	{
		return;
	}
	myCurrentAction = eCurrentAction::eTurningLeft;
	if (myDriftSetupTimer < myDriftSetupTime)
	{
		if (myDrifter->IsDrifting() == false)
		{
			Drift();
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

	if (mySteering >= 0.f)
	{
		myAnimator->OnTurnLeft(aNormalizedModifier);
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
	myAcceleration = GetMaxAcceleration();
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
	if (myCurrentAction == eCurrentAction::eTurningLeft/*mySteering < 0.f*/)
	{
		myAnimator->OnStopTurningLeft();
	}
	else if (myCurrentAction == eCurrentAction::eTurningRight /*mySteering > 0.f*/)
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
	if (myVelocity.Length2() < GetMaxSpeed2() * 0.33f)
	{
		return false;
	}
	myDrifter->StartDrifting(myCurrentAction);
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

	return true;
}

void CKartControllerComponent::StopDrifting()
{
	myDriftSetupTimer = myDriftSetupTime + 1.0f;
	GetParent()->NotifyComponents(eComponentMessageType::eCancelDriftBobbing, SComponentMessageData());
	CDrifter::eDriftBoost boost = myDrifter->StopDrifting();

	if (myControllerHandle != -1 && myControllerHandle < 4)
	{
		StopVibrationOnController* stopionMessageLeft = new StopVibrationOnController(myControllerHandle);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(stopionMessageLeft);
	}
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
		StopDrifting();
		GetParent()->NotifyComponents(eComponentMessageType::eSpinKart, SComponentMessageData());
		CParticleEmitterManager::GetInstance().Activate(myGotHitEmmiterhandle);
		SComponentMessageData sound; sound.myString = "PlayGetHit";
		GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
	}
	//myAcceleration = 0;
}

void CKartControllerComponent::ApplyStartBoost()
{
	if (myPreRaceBoostValue > myPreRaceBoostTarget * 0.6)
	{
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
			return;
		}
		SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("MiniDriftBoost"));
		GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
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
	SComponentMessageData messageData;

	myIsOnGround = true;
	myCanAccelerate = true;
	DoPhysics(aDeltaTime);
	CheckWallKartCollision(aDeltaTime);
	CheckZKill();

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
		if (myElapsedInvurnableTime >= myInvurnableTime)
		{
			myElapsedInvurnableTime = 0;
			myIsInvurnable = false;
			SComponentMessageData sound; sound.myString = "StopStar";
			GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eTurnOffHazard, SComponentMessageData());
		}
	}

	GetParent()->NotifyComponents(eComponentMessageType::eMoving, messageData);
	myAnimator->Update(aDeltaTime);
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
		myIsInvurnable = true;
		myInvurnableTime = aMessageData.myBoostData->duration;
		SComponentMessageData sound; sound.myString = "PlayStar";
		GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
		break;
	}
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
		myBoostSpeedDecay = GetMaxAcceleration() * myAccelerationModifier * 1.25f;
		break;
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
	const CU::Vector3f forwardVector = GetParent()->GetToWorldTransform().myForwardVector;
	const float speed = forwardVector.Dot(myVelocity);
	const float dir = myVelocity.Dot(forwardVector);

	float way = 1.f;
	if (dir > 0.f)
	{
		way = -1.f;
	}

	const float onGroundModifier = (myCanAccelerate == true ? 1.f : 0.f);
	const float gripOverWeight = (myGrip / myWeight);
	myVelocity -= myVelocity * myGrip * aDeltaTime * onGroundModifier;

	if (myHasGottenHit == false)
	{
		myVelocity += forwardVector * aDeltaTime * GetAcceleratiot() * myGrip * myAccelerationModifier * onGroundModifier;
	}
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
	float steerAngle = 0.f;
	GetParent()->Move(CU::Vector3f::UnitZ * speed * myDrifter->GetDriftBonusSpeed() * aDeltaTime);
	GetParent()->Move(CU::Vector3f::UnitY * myVelocity.y * myDrifter->GetDriftBonusSpeed() * aDeltaTime);
	if (myDrifter->IsDrifting() == true)
	{
		myDrifter->UpdateDriftParticles(GetParent()->GetLocalTransform());
		myDrifter->GetSteering(steerAngle,myTurnRate,myAngularAcceleration,way,myAirControl,myIsOnGround,myCurrentAction,aDeltaTime);
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
			static_cast<Physics::ECollisionLayer>(Physics::eWall/* | Physics::eKart*/));


		if (raycastHitData.hit == true)
		{
			/*if(raycastHitData.collisionLayer == Physics::eWall)
			{*/
				GetParent()->Move(raycastHitData.normal * (raycastHitData.distance - testDist) * -2.f);
				myVelocity *= 0.75f;
				const float repulsion = CLAMP(myVelocity.Length() * 50.f, 0.f, GetMaxSpeed() * 2.f);
				myVelocity += repulsion * raycastHitData.normal;
			
			if(raycastHitData.collisionLayer == Physics::eKart)
			{
				int i = 0;
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

	Physics::SRaycastHitData raycastHitData = myPhysicsScene->Raycast(examineVector + upMove, down, testLength, Physics::eGround);

	CU::Vector3f downAccl = down;
	float friction = 1.f;
	if (raycastHitData.hit == true)
	{
		const CU::Vector3f& norm = raycastHitData.normal;
		if(raycastHitData.distance < controlDist)
		{
			myIsOnGround = true;
			if (myHasJumped == true)
			{
				Drift();
				myHasJumped = false;
			}
		}
		if (raycastHitData.distance < onGroundDist)
		{
			friction = 0.f;
			myCanAccelerate = true;

			CComponent* component = reinterpret_cast<CComponent*>(raycastHitData.actor->GetCallbackData()->GetUserData())->GetParent();

			if(component != myLastGroundComponent)
			{
				myLastGroundComponent = component;
				SComponentQuestionData questionData;
				if(myLastGroundComponent->Answer(eComponentQuestionType::eGetTerrainModifier, questionData))
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
	else
	{
		myTerrainModifier = 1.f;
		myLastGroundComponent = nullptr;
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
	default:
		break;
	}
	return false;
}
