#pragma once


enum class eCurrentAction;

namespace CU
{
	class CJsonValue;
}

class CDrifter
{
public:
	enum class eDriftState
	{
		eDriftingLeft,
		eDriftingRight,
		eNotDrifting,
	};

	enum class eDriftBoost
	{
		eLarge,
		eSmall,
		eNone
	};

	CDrifter();
	CDrifter(const CDrifter& aCopy) = default;
	~CDrifter();

	bool Init(const CU::CJsonValue& aJsonValue);
	void GetSteering(float& aSteeringAngle, const float aTurnRate, const float aAngularAcceleration, const float aWay, const float aAirControll, const bool aIsOnGround, const eCurrentAction& aAction, const float aDeltaTime);

	void StartDrifting(const eCurrentAction& aCurrentAction);
	eDriftBoost StopDrifting();

	void TurnRight();
	void TurnLeft();
	void StopTurning();
	void UpdateDriftParticles(const CU::Matrix44f& aKartOrientation);

	inline float GetDriftRate() const;
	inline float GetSteerModifier() const;
	inline bool IsDrifting() const;
	inline bool WheelsAreBurning() const;


	inline const float GetDriftBonusSpeed() const;

private:
	void SetDriftParticlesReady(const bool aFlag);
	void SetSmallBoostReady(const bool aFlag);
	void SetLargeBoostReady(const bool aFlag);


	float myDriftRate;
	float myDriftTimer;
	float myDriftSteerModifier;
	float myDriftSteeringModifier;
	float myMaxDriftRate;
	float myTimeMultiplier;
	float myMaxDriftSteerAffection;

	float mySlowExtraSpeed;
	float myFastExtraSpeed;

	float myLongDriftTime;
	float myShortDriftTime;

	int myLeftDriftDustEmitterHandle;
	int myRightDriftDustEmitterHandle;
	int myLeftSmallBoostReadyEmitterHandle;
	int myRightSmallBoostReadyEmitterHandle;
	int myLeftLargeBoostReadyEmitterHandle;
	int myRightLargeBoostReadyEmitterHandle;

	eDriftState myDriftState;

	bool myIsDrifting;
};

inline float CDrifter::GetDriftRate() const
{
	return myDriftRate;
}

inline float CDrifter::GetSteerModifier() const
{
	return myDriftSteerModifier;
}

inline bool CDrifter::IsDrifting() const
{
	return myIsDrifting;
}

inline bool CDrifter::WheelsAreBurning() const
{
	return myDriftTimer > myShortDriftTime;
}

inline const float CDrifter::GetDriftBonusSpeed() const
{
	if (myDriftTimer > myShortDriftTime)
	{
		if (myDriftTimer > myLongDriftTime)
		{
			return myFastExtraSpeed;
		}
		return mySlowExtraSpeed;
	}
	return 1.0f;
}
