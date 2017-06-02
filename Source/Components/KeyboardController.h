#pragma once
#include "Controller.h"
#include "../CommonUtilities/InputListener.h"

class CAIController;

class CKeyboardController : public CController, public CU::IInputListener
{
public:
	CKeyboardController(CKartControllerComponent& aKartComponent);
	~CKeyboardController();

	virtual CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

private:
	void ReleasedKey(const CU::SInputMessage& aInputMessage);
	void PressedKey(const CU::SInputMessage& aInputMessage);

	CAIController* myAIController;

	bool myIsMovingFoward;
	bool myIsMovingBackwards;
	bool myIsTurningLeft;
	bool myIsTurningRight;
};
