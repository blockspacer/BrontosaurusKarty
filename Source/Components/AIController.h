#pragma once
#include "Controller.h"

class CNavigationSpline;

class CAIController : public CController
{
	enum class eStates : char
	{
		eForward, 
		eStop,
		eBackward
	};

public:
	CAIController(CKartControllerComponent& aKartComponent);
	~CAIController();

	void Update(const float aDeltaTime) override;
	void UpdateWithoutItems(const float aDeltaTime);

private:
	void UpdateItemUsage(const float aDeltaTime);
	void UpdateMovement(const float aDeltaTime, const float aMaxSpeedModifier);
	const CNavigationSpline& GetNavigationSpline();

	int myCurrentSplineIndex;
	float myUseItemTimer;

	eStates myState;
	char myIsDrifting;
	bool myHasItem;
};

