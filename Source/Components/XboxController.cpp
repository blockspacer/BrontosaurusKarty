#include "stdafx.h"
#include "XboxController.h"
#include "KartControllerComponent.h"

#include "SpeedHandlerManager.h"
#include "BoostData.h"

#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EInputReturn.h"
#include "..\CommonUtilities\XInputWrapper.h"
#include "..\CommonUtilities\EKeyboardKeys.h"

#include "..\PostMaster\SetVibrationOnController.h"
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
	else
	{
		DL_PRINT("my index is : %d", myControllerIndex);
	}

	return CU::eInputReturn::ePassOn;
}

void CXboxController::GamePadPressedKey(const CU::SInputMessage & aInputMessage)
{
	SComponentMessageData boostMessageData;
	SBoostData* boostData = new SBoostData();

	switch (aInputMessage.myGamePad)
	{
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
	case CU::GAMEPAD::LEFT_SHOULDER:
		boostData->accerationBoost = 5;
		boostData->duration = 4.0f;
		boostData->maxSpeedBoost = 2.0f;
		boostData->hashedName = std::hash<std::string>()("TempBoost");
		boostMessageData.myBoostData = boostData;
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
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
	case CU::GAMEPAD::LEFT_SHOULDER:
		myControllerComponent.StopDrifting();
		break;
	}
}

void CXboxController::MovedJoystick(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myJoyStickPosition.x == 0.f)
	{
		myControllerComponent.StopTurning();
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
	SComponentMessageData boostMessageData;
	SBoostData* boostData = new SBoostData();
	boostData->accerationBoost = 5;
	boostData->duration = 4.0f;
	boostData->maxSpeedBoost = 2.0f;
	boostData->hashedName = std::hash<std::string>()("TempBoost");
	boostMessageData.myBoostData = boostData;
	myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
}

void CXboxController::GamePadRightTrigger(const CU::SInputMessage& aInputMessage)
{
	if (myIsDrifting == false)
	{
		myControllerComponent.Drift();

		SetVibrationOnController* vibrationMessage = new SetVibrationOnController(myControllerIndex, 10, 10);
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(vibrationMessage);
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