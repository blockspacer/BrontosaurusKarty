#include "stdafx.h"
#include "KeyboardControllerComponent.h"
#include "..\CommonUtilities\EInputReturn.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EKeyboardKeys.h"

const float ForwardAcceleration = 2.f;

CKeyboardControllerComponent::CKeyboardControllerComponent()
{
}

CKeyboardControllerComponent::~CKeyboardControllerComponent()
{
}

CU::eInputReturn CKeyboardControllerComponent::TakeInput(const CU::SInputMessage& aInputMessage)
{
	CGameObject* parent = GetParent();
	if (!parent)
	{
		return CU::eInputReturn::ePassOn;
	}

	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		SComponentMessageData moveData;

		switch (aInputMessage.myKey)
		{
		case CU::eKeys::UP:
		case CU::eKeys::W:
			moveData.myFloat = ForwardAcceleration;
			parent->NotifyComponents(eComponentMessageType::eAccelerate, moveData); //change eActivate to w/e marcus' has written
			break;
		case CU::eKeys::DOWN:
		case CU::eKeys::S:
			moveData.myFloat = -ForwardAcceleration;
			parent->NotifyComponents(eComponentMessageType::eActivate, moveData);
			break;
		case CU::eKeys::LEFT:
		case CU::eKeys::A:
			break;
		case CU::eKeys::RIGHT:
		case CU::eKeys::D:
			break;
		}
	}

	return CU::eInputReturn::ePassOn;
}
