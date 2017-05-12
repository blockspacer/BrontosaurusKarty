#pragma once
#include "Controller.h"
#include "..\CommonUtilities\InputListener.h"

class CXboxController : public CController, public CU::IInputListener
{
public:
	CXboxController(CKartControllerComponent& aKartComponent);
	~CXboxController();

	void SetIndex(const int aIndex);
	virtual CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

private:
	void GamePadPressedKey(const CU::SInputMessage& aInputMessage);
	void GamePadReleasedKey(const CU::SInputMessage& aInputMessage);
	void MovedJoystick(const CU::SInputMessage& aInputMessage);
	void GamePadLeftTrigger(const CU::SInputMessage& aInputMessage);
	void GamePadRightTrigger(const CU::SInputMessage& aInputMessage);
	void GamePadLeftTriggerReleased(const CU::SInputMessage& aInputMessage);
	void GamePadRightTriggerReleased(const CU::SInputMessage& aInputMessage);
	void JoystickDeadzone();

	short myControllerIndex;
	
	bool myIsMovingFoward;
	bool myIsMovingBackwards;
	bool myIsDrifting;
};
