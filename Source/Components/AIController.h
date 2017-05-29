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

private:
	const CNavigationSpline& GetNavigationSpline();
	int myCurrentSplineIndex;
	float myTimer;

	eStates myState;
	char myIsDrifting;
};

