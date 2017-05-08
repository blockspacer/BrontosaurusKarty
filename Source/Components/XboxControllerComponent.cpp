#include "stdafx.h"
#include "XboxControllerComponent.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EInputReturn.h"
#include "..\CommonUtilities\XInputWrapper.h"


CXboxControllerComponent::CXboxControllerComponent()
{
}

CXboxControllerComponent::~CXboxControllerComponent()
{
}

CU::eInputReturn CXboxControllerComponent::TakeInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eGamePadButtonPressed)
	{
		PressedKey(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eGamePadButtonReleased)
	{
		ReleasedKey(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eGamePadJoyStickChanged)
	{
		MovedJoystick(aInputMessage);
	}

	return CU::eInputReturn::ePassOn;
}

void CXboxControllerComponent::PressedKey(const CU::SInputMessage& aInputMessage)
{
	CGameObject* parent = GetParent();
	if (!parent)
	{
		return;
	}

	SComponentMessageData moveData;

	switch (aInputMessage.myGamePad)
	{
	case CU::GAMEPAD::A:
		parent->NotifyComponents(eComponentMessageType::eAccelerate, moveData);
		break;
	case CU::GAMEPAD::B:
		parent->NotifyComponents(eComponentMessageType::eDecelerate, moveData);
		break;
	}
}

void CXboxControllerComponent::ReleasedKey(const CU::SInputMessage& aInputMessage)
{
	CGameObject* parent = GetParent();
	if (!parent)
	{
		return;
	}

	SComponentMessageData moveData;

	switch (aInputMessage.myGamePad)
	{
	case CU::GAMEPAD::A:
		parent->NotifyComponents(eComponentMessageType::eStopAcceleration, moveData);
		break;
	case CU::GAMEPAD::B:
		parent->NotifyComponents(eComponentMessageType::eStopDeceleration, moveData);
		break;
	}
}

void CXboxControllerComponent::MovedJoystick(const CU::SInputMessage& aInputMessage)
{
	CGameObject* parent = GetParent();
	if (!parent)
	{
		return;
	}

	SComponentMessageData moveData;
	eComponentMessageType moveType = eComponentMessageType::eTurnKart;
	moveData.myFloat = aInputMessage.myJoyStickPosition.x;
	if (aInputMessage.myJoyStickPosition.x == 0.f)
	{
		moveType = eComponentMessageType::eStopTurnKart;
	}

	parent->NotifyComponents(moveType, moveData);
}

void CXboxControllerComponent::JoystickDeadzone()
{
	CGameObject* parent = GetParent();
	if (!parent)
	{
		return;
	}

	static const SComponentMessageData nullData;

	parent->NotifyComponents(eComponentMessageType::eStopTurnKart, nullData);
}
