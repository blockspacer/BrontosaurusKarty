#include "stdafx.h"
#include "XboxController.h"
#include "KartControllerComponent.h"

#include "SpeedHandlerManager.h"
#include "BoostData.h"

#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EInputReturn.h"
#include "..\CommonUtilities\XInputWrapper.h"
#include "..\CommonUtilities\EKeyboardKeys.h"

#include "..\ThreadedPostmaster\SetVibrationOnController.h"
#include "..\ThreadedPostmaster\Postmaster.h"

CXboxController::CXboxController(CKartControllerComponent& aKartComponent)
	: CController(aKartComponent)
	, myControllerIndex(-1)
	, myIsMovingFoward(false)
	, myIsMovingBackwards(false)
	, myIsDrifting(false)
{
}

CXboxController::~CXboxController()
{
}

void CXboxController::SetIndex(const int aIndex)
{
	myControllerIndex = static_cast<short>(aIndex);
}

CU::eInputReturn CXboxController::TakeInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myGamepadIndex == myControllerIndex)
	{
		switch (aInputMessage.myType)
		{
		case CU::eInputType::eGamePadButtonPressed:
			GamePadPressedKey(aInputMessage);
			break;
		case CU::eInputType::eGamePadButtonReleased:
			GamePadReleasedKey(aInputMessage);
			break;
		case CU::eInputType::eGamePadLeftJoyStickChanged:
			MovedJoystick(aInputMessage);
			break;
		case CU::eInputType::eGamePadLeftTriggerPressed:
			GamePadLeftTrigger(aInputMessage);
			break;
		case CU::eInputType::eGamePadRightTriggerPressed:
			GamePadRightTrigger(aInputMessage);
			break;
		case CU::eInputType::eGamePadLeftTriggerReleased:
			GamePadLeftTriggerReleased(aInputMessage);
			break;
		case CU::eInputType::eGamePadRightTriggerReleased:
			GamePadRightTriggerReleased(aInputMessage);
			break;
		}
	}

	return CU::eInputReturn::ePassOn;
}

void CXboxController::GamePadPressedKey(const CU::SInputMessage & aInputMessage)
{
	SComponentMessageData boostMessageData;
	SBoostData* boostData = new SBoostData();

	switch (aInputMessage.myGamePad)
	{
	case CU::GAMEPAD::DPAD_DOWN:
	{
		SComponentMessageData data;
		data.myInt = 0; //GreenShell
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::DPAD_LEFT:
	{
		SComponentMessageData data;
		data.myInt = 2; //mushroom
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::DPAD_RIGHT:
	{
		SComponentMessageData data;
		data.myInt = 4;//Banana
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::DPAD_UP:
	{
		SComponentMessageData data;
		data.myInt = 3; //Star
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::A:
		myControllerComponent.MoveFoward();
		myIsMovingFoward = true;
		break;
	case CU::GAMEPAD::B:
		myControllerComponent.MoveBackWards();
		myIsMovingBackwards = true;
		break;
	case CU::GAMEPAD::RIGHT_SHOULDER:
		myControllerComponent.Drift();
		break;
	case CU::GAMEPAD::LEFT_THUMB:
	{
		/*SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPowerUp"));
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);*/
	}
	case CU::GAMEPAD::LEFT_SHOULDER:
		/*SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPad"));
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);*/

		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eUseItem, SComponentMessageData());
		break;
	}
}

void CXboxController::GamePadReleasedKey(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myGamePad)
	{
	case CU::GAMEPAD::A:
		myControllerComponent.StopMoving();
		myIsMovingFoward = false;
		if (myIsMovingBackwards == true)
		{
			myControllerComponent.MoveBackWards();
		}
		break;
	case CU::GAMEPAD::B:
		myControllerComponent.StopMoving();
		myIsMovingBackwards = false;
		if (myIsMovingFoward == true)
		{
			myControllerComponent.MoveFoward();
		}
		break;
	case CU::GAMEPAD::RIGHT_SHOULDER:
		myControllerComponent.StopDrifting();
		break;
		break;
	}
}

void CXboxController::MovedJoystick(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myJoyStickPosition.x == 0.f)
	{
		if (aInputMessage.myJoyStickPosition.y == 0.f)
		{
			myControllerComponent.StopTurning();
		}
	}
	else
	{
		myControllerComponent.Turn(aInputMessage.myJoyStickPosition.x);
	}

	//if (aInputMessage.myJoyStickPosition.x > 0)
	//{
	//	myControllerComponent.TurnRight();
	//}
	//else if (aInputMessage.myJoyStickPosition.x < 0)
	//{
	//	myControllerComponent.TurnLeft();
	//}
}

void CXboxController::GamePadLeftTrigger(const CU::SInputMessage& aInputMessage)
{
	myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eUseItem, SComponentMessageData());
}

void CXboxController::GamePadRightTrigger(const CU::SInputMessage& aInputMessage)
{
	if (myIsDrifting == false)
	{
		myControllerComponent.Drift();
		myIsDrifting = true;
	}
}

void CXboxController::GamePadLeftTriggerReleased(const CU::SInputMessage& aInputMessage)
{
}

void CXboxController::GamePadRightTriggerReleased(const CU::SInputMessage& aInputMessage)
{
	if (myIsDrifting == true)
	{
		myControllerComponent.StopDrifting();
		myIsDrifting = false;
	}
}

void CXboxController::JoystickDeadzone()
{
	myControllerComponent.StopTurning();
}