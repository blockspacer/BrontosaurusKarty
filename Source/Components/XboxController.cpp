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

#include "AIController.h"
#include "..\BrontosaurusEngine\Engine.h"
#include "../ThreadedPostmaster/ControllerInputMessage.h"

CXboxController::CXboxController(CKartControllerComponent& aKartComponent)
	: CController(aKartComponent)
	, myControllerIndex(-1)
	, myIsMovingFoward(false)
	, myIsMovingBackwards(false)
	, myIsDrifting(false)
	, myLeftTrigger(false)
{
	myAIController = new CAIController(aKartComponent);
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
	if (!myControllerComponent.GetParent())
	{
		return CU::eInputReturn::ePassOn;
	}
	if (myControllerComponent.GetIsControlledByAI())
	{
		return CU::eInputReturn::ePassOn;
	}

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
		data.myInt = 0; //Star
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::DPAD_LEFT:
	{
		SComponentMessageData data;
		data.myInt = 1; //Star
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::DPAD_RIGHT:
	{
		SComponentMessageData data;
		data.myInt = 6; //Star
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::DPAD_UP:
	{
		SComponentMessageData data;
		data.myInt = 4; //Star
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
		break;
	}
	case CU::GAMEPAD::X:
	{
		break;
	}
	case CU::GAMEPAD::Y:
	{

		break;
	}
	case CU::GAMEPAD::A:
		myControllerComponent.MoveFoward();
		myControllerComponent.IncreasePreGameBoostValue();
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
		break;
	}
	case CU::GAMEPAD::LEFT_SHOULDER:
		/*SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPad"));
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);*/

		//myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eUseItem, SComponentMessageData());
		myControllerComponent.LookBack(true);
		break;
	}
}

void CXboxController::GamePadReleasedKey(const CU::SInputMessage& aInputMessage)
{
	switch (aInputMessage.myGamePad)
	{
	case CU::GAMEPAD::A:
		myControllerComponent.StopMoving();
		myControllerComponent.DecreasePreGameBoostValue();
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
		myControllerComponent.StopDrifting(true);
		break;
	case CU::GAMEPAD::LEFT_SHOULDER:
		myControllerComponent.LookBack(false);
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

	myJoyStickInput = aInputMessage.myJoyStickPosition;
	
}

void CXboxController::GamePadLeftTrigger(const CU::SInputMessage& aInputMessage)
{
	if (myLeftTrigger == false)
	{
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eUseItem, SComponentMessageData());
		myLeftTrigger = true;
	}
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
	if (myLeftTrigger == true)
	{
	/*	SReleaseItemData data;
		data.userComponent = &myControllerComponent;
		data.direction = myJoyStickInput;*/
		SComponentMessageData input;
		input.myVector2f = myJoyStickInput;
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eReleaseItem, input);
		myLeftTrigger = false;
	}
}

void CXboxController::GamePadRightTriggerReleased(const CU::SInputMessage& aInputMessage)
{
	if (myIsDrifting == true)
	{
		myControllerComponent.StopDrifting(true);
		myIsDrifting = false;
	}
}

void CXboxController::JoystickDeadzone()
{
	myControllerComponent.StopTurning();
}


	

