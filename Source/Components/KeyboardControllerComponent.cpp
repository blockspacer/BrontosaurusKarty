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

	SComponentMessageData moveData;

	switch (aInputMessage.myKey)
	{
	case CU::eKeys::UP:
	case CU::eKeys::W:
		parent->NotifyComponents(eComponentMessageType::eAccelerate, moveData);
		break;
	case CU::eKeys::DOWN:
	case CU::eKeys::S:
		parent->NotifyComponents(eComponentMessageType::eDecelerate, moveData);
		break;
	case CU::eKeys::LEFT:
	case CU::eKeys::A:
		moveData.myFloat = -1.f;
		parent->NotifyComponents(eComponentMessageType::eTurnKart, moveData);
		break;
	case CU::eKeys::RIGHT:
	case CU::eKeys::D:
		moveData.myFloat = 1.f;
		parent->NotifyComponents(eComponentMessageType::eTurnKart, moveData);
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

	SComponentMessageData moveData;

	switch (aInputMessage.myKey)
	{
	case CU::eKeys::UP:
	case CU::eKeys::W:
		parent->NotifyComponents(eComponentMessageType::eStopAcceleration, moveData);
		break;
	case CU::eKeys::DOWN:
	case CU::eKeys::S:
		parent->NotifyComponents(eComponentMessageType::eStopDeceleration, moveData);
		break;
	case CU::eKeys::LEFT:
	case CU::eKeys::A:
		moveData.myFloat = 1.f;
		parent->NotifyComponents(eComponentMessageType::eTurnKart, moveData);
		break;
	case CU::eKeys::RIGHT:
	case CU::eKeys::D:
		moveData.myFloat = -1.f;
		parent->NotifyComponents(eComponentMessageType::eTurnKart, moveData);
		break;
	}
}
