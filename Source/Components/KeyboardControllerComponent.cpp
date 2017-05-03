#include "stdafx.h"
#include "KeyboardControllerComponent.h"
#include "..\CommonUtilities\EInputReturn.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EKeyboardKeys.h"

CKeyboardControllerComponent::CKeyboardControllerComponent()
{
}

CKeyboardControllerComponent::~CKeyboardControllerComponent()
{
}

CU::eInputReturn CKeyboardControllerComponent::TakeInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		PressedKey(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eKeyboardReleased)
	{
		ReleasedKey(aInputMessage);
	}

	return CU::eInputReturn::ePassOn;
}

void CKeyboardControllerComponent::PressedKey(const CU::SInputMessage& aInputMessage)
{
	CGameObject* parent = GetParent();
	if (!parent)
	{
		return;
	}

	static const SComponentMessageData nullData;

	switch (aInputMessage.myKey)
	{
	case CU::eKeys::UP:
	case CU::eKeys::W:
		parent->NotifyComponents(eComponentMessageType::eAccelerate, nullData);
		break;
	case CU::eKeys::DOWN:
	case CU::eKeys::S:
		parent->NotifyComponents(eComponentMessageType::eDecelerate, nullData);
		break;
	case CU::eKeys::LEFT:
	case CU::eKeys::A:
		break;
	case CU::eKeys::RIGHT:
	case CU::eKeys::D:
		break;
	}
}

void CKeyboardControllerComponent::ReleasedKey(const CU::SInputMessage& aInputMessage)
{
	CGameObject* parent = GetParent();
	if (!parent)
	{
		return;
	}

	static const SComponentMessageData nullData;

	switch (aInputMessage.myKey)
	{
	case CU::eKeys::UP:
	case CU::eKeys::W:
		parent->NotifyComponents(eComponentMessageType::eStopAcceleration, nullData);
		break;
	case CU::eKeys::DOWN:
	case CU::eKeys::S:
		parent->NotifyComponents(eComponentMessageType::eStopDeceleration, nullData);
		break;
	case CU::eKeys::LEFT:
	case CU::eKeys::A:
		break;
	case CU::eKeys::RIGHT:
	case CU::eKeys::D:
		break;
	}
}
