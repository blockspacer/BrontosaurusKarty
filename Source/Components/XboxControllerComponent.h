#pragma once
#include "Component.h"
#include "../CommonUtilities/InputListener.h"

class CXboxControllerComponent : public CComponent, public CU::IInputListener
{
public:
	CXboxControllerComponent();
	~CXboxControllerComponent();

	virtual CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

private:
	void PressedKey(const CU::SInputMessage& aInputMessage);
	void ReleasedKey(const CU::SInputMessage& aInputMessage);
	void MovedJoystick(const CU::SInputMessage& aInputMessage);
	void JoystickDeadzone();

	float myLastTurnDirection;
};
