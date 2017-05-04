#pragma once
#include "Controller.h"
#include "../CommonUtilities/InputListener.h"
class CPlayerController : public CController, public CU::IInputListener
{
public:
	CPlayerController(CKartControllerComponent& aKartComponent);
	~CPlayerController();

	void Update(const float aDeltaTime) override;

	virtual CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

private:

	void ReleasedKey(const CU::SInputMessage& aInputMessage);
	void PressedKey(const CU::SInputMessage& aInputMessage);
	void GamePadPressedKey(const CU::SInputMessage& aInputMessage);
	void GamePadReleasedKey(const CU::SInputMessage& aInputMessage);
	void MovedJoystick(const CU::SInputMessage& aInputMessage);
	void JoystickDeadzone();

	bool myIsMovingFoward;
	bool myIsMovingBackwards;
	bool myIsTurningLeft;
	bool myIsTurningRight;


	float myLastTurnDirection;
};

