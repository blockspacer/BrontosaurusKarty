#pragma once
#include "Component.h"
#include "../CommonUtilities/InputListener.h"

class CKeyboardControllerComponent : public CComponent, public CU::IInputListener
{
public:
	CKeyboardControllerComponent();
	~CKeyboardControllerComponent();


	virtual CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

private:
	void PressedKey(const CU::SInputMessage& aInputMessage);
	void ReleasedKey(const CU::SInputMessage& aInputMessage);

};

