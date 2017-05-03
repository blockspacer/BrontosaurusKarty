#pragma once
#include "Component.h"
#include "../CommonUtilities/InputListener.h"

class CKeyboardControllerComponent : public CComponent, public CU::IInputListener
{
public:
	CKeyboardControllerComponent();
	~CKeyboardControllerComponent();


	virtual CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;

};

