#include "stdafx.h"
#include "PlayerController.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EInputReturn.h"
#include "..\CommonUtilities\XInputWrapper.h"
#include "..\CommonUtilities\EKeyboardKeys.h"
#include "KartControllerComponent.h"

//Temp includes
#include "BoostData.h"

CPlayerController::CPlayerController(CKartControllerComponent & aKartComponent) : CController(aKartComponent)
{
	myIsMovingBackwards = false;
	myIsMovingFoward = false;
	myIsTurningLeft = false;
	myIsTurningRight = false;
}

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Update(const float aDeltaTime)
{
	//kolla input och kalla p� functioner hos KartControllerComponent
}

CU::eInputReturn CPlayerController::TakeInput(const CU::SInputMessage & aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eGamePadButtonPressed)
	{
		GamePadPressedKey(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eGamePadButtonReleased)
	{
		GamePadReleasedKey(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eGamePadLeftJoyStickChanged)
	{
		MovedJoystick(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eKeyboardReleased)
	{
		ReleasedKey(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		PressedKey(aInputMessage);
	}

	return CU::eInputReturn::ePassOn;
}

void CPlayerController::ReleasedKey(const CU::SInputMessage & aInputMessage)
{
	switch (aInputMessage.myKey)
	{
	case CU::eKeys::UP:
	case CU::eKeys::W:
		myControllerComponent.StopMoving();
		myIsMovingFoward = false;
		if (myIsMovingBackwards == true)
		{
			myControllerComponent.MoveBackWards();
		}
		break;
	case CU::eKeys::DOWN:
	case CU::eKeys::S:
		myControllerComponent.StopMoving();
		myIsMovingBackwards = false;
		if (myIsMovingFoward == true)
		{
			myControllerComponent.MoveFoward();
		}
		break;
	case CU::eKeys::LEFT:
	case CU::eKeys::A:
		myControllerComponent.StopTurning();
		myIsTurningLeft = false;
		if (myIsTurningRight == true)
		{
			myControllerComponent.TurnRight();
		}
		break;
	case CU::eKeys::RIGHT:
	case CU::eKeys::D:
		myControllerComponent.StopTurning();
		myIsTurningRight = false;
		if (myIsTurningLeft == true)
		{
			myControllerComponent.TurnLeft();
		}
		break;
	case CU::eKeys::LCONTROL:
		myControllerComponent.StopDrifting();
		break;
	}
}

void CPlayerController::PressedKey(const CU::SInputMessage & aInputMessage)
{
	switch (aInputMessage.myKey)
	{
	case CU::eKeys::UP:
	case CU::eKeys::W:
		myControllerComponent.MoveFoward();
		myIsMovingFoward = true;
		break;
	case CU::eKeys::DOWN:
	case CU::eKeys::S:
		myControllerComponent.MoveBackWards();
		myIsMovingBackwards = true;
		break;
	case CU::eKeys::LEFT:
	case CU::eKeys::A:
		myControllerComponent.TurnLeft();
		myIsTurningLeft = true;
		break;
	case CU::eKeys::RIGHT:
	case CU::eKeys::D:
		myControllerComponent.TurnRight();
		myIsTurningRight = true;
		break;
	case CU::eKeys::SPACE:
	{
		SComponentMessageData boostMessageData;
		SBoostData* boostData = new SBoostData();
		boostData->accerationBoost = 5;
		boostData->duration = 4.0f;
		boostData->maxSpeedBoost = 2.0f;
		boostData->type = eBoostType::eDefault;
		boostMessageData.myBoostData = boostData;
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		break;
	}
	case CU::eKeys::LCONTROL:
		myControllerComponent.Drift();
		break;
	}
}

void CPlayerController::GamePadPressedKey(const CU::SInputMessage & aInputMessage)
{
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
		SComponentMessageData boostMessageData;
		SBoostData* boostData = new SBoostData();
		boostData->accerationBoost = 5;
		boostData->duration = 4.0f;
		boostData->maxSpeedBoost = 2.0f;
		boostData->type = eBoostType::eDefault;
		boostMessageData.myBoostData = boostData;
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
		break;
	}
}

void CPlayerController::GamePadReleasedKey(const CU::SInputMessage & aInputMessage)
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
	}
}

void CPlayerController::MovedJoystick(const CU::SInputMessage & aInputMessage)
{
	if (aInputMessage.myJoyStickPosition.x == 0.f)
	{
		myControllerComponent.StopTurning();
		return;
	}

	if (aInputMessage.myJoyStickPosition.x > 0)
	{
		myControllerComponent.TurnRight();
	}
	else if(aInputMessage.myJoyStickPosition.x < 0)
	{
		myControllerComponent.TurnLeft();
	}

}

void CPlayerController::JoystickDeadzone()
{
	myControllerComponent.StopTurning();
}
