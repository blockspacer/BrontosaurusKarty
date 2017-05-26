#pragma once


enum class eCurrentAction;

namespace CU
{
	class CJsonValue;
}

class CDrifter
{
public:
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
	void Update(const float aDeltaTime);

	void StartDrifting(const eCurrentAction& aCurrentAction);
	eDriftBoost StopDrifting();

	void TurnRight();
	void TurnLeft();
	void StopTurning();

	inline float GetDriftRate() const;
	inline float GetSteerModifier() const;
	inline bool IsDrifting() const;
	inline bool WheelsAreBurning() const;

private:
	float myDriftRate;
	float myDriftTimer;
	float myDriftSteerModifier;
	float myDriftSteeringModifier;
	float myMaxDriftRate;
	float myTimeMultiplier;
	float myMaxDriftSteerAffection;

	float myLongDriftTime;
	float myShortDriftTime;

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
