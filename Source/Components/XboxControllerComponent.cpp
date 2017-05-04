#include "stdafx.h"
#include "XboxControllerComponent.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EInputReturn.h"


CXboxControllerComponent::CXboxControllerComponent()
{
}

CXboxControllerComponent::~CXboxControllerComponent()
{
}

CU::eInputReturn CXboxControllerComponent::TakeInput(const CU::SInputMessage& aInputMessage)
{
	//if (aInputMessage.myType == CU::eInputType::eGamePadPressed)
	//{
	//	PressedKey(aInputMessage);
	//}
	//else if (aInputMessage.myType == CU::eInputType::eGamePadReleased)
	//{
	//	ReleasedKey(aInputMessage);
	//}
	//else if (aInputMessage.myType == CU::eInputType::eJoystickMoved)
	//{
	//	MovedJoystick(aInputMessage);
	//}

	return CU::eInputReturn::ePassOn;
}

void CXboxControllerComponent::PressedKey(const CU::SInputMessage& aInputMessage)
{
	//CGameObject* parent = GetParent();
	//if (!parent)
	//{
	//	return;
	//}

	//SComponentMessageData moveData;

	//switch (aInputMessage.myKey)
	//{
	//case CU::GAMEPAD::A:
	//	parent->NotifyComponents(eComponentMessageType::eAccelerate, moveData);
	//	break;
	//case CU::GAMEPAD::B:
	//	parent->NotifyComponents(eComponentMessageType::eDecelerate, moveData);
	//	break;
	//}
}

void CXboxControllerComponent::ReleasedKey(const CU::SInputMessage& aInputMessage)
{
	//CGameObject* parent = GetParent();
	//if (!parent)
	//{
	//	return;
	//}

	//SComponentMessageData moveData;

	//switch (aInputMessage.myKey)
	//{
	//case CU::GAMEPAD::A:
	//	parent->NotifyComponents(eComponentMessageType::eAccelerate, moveData);
	//	break;
	//case CU::GAMEPAD::B:
	//	parent->NotifyComponents(eComponentMessageType::eDecelerate, moveData);
	//	break;
	//}
}

void CXboxControllerComponent::MovedJoystick(const CU::SInputMessage& aInputMessage)
{
	//CGameObject* parent = GetParent();
	//if (!parent)
	//{
	//	return;
	//}

	//SComponentMessageData moveData;

	//moveData.myFloat = aInputMessage.myJoystickPosition.x;
	//parent->NotifyComponents(eComponentMessageType::eTurnKart, moveData);
}

void CXboxControllerComponent::JoystickDeadzone()
{
	//CGameObject* parent = GetParent();
	//if (!parent)
	//{
	//	return;
	//}

	//static const SComponentMessageData nullData;

	//parent->NotifyComponents(eComponentMessageType::eStopTurnKart, nullData);
}
