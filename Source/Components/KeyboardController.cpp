#include "stdafx.h"
#include "KeyboardController.h"
#include "..\CommonUtilities\InputMessage.h"
#include "..\CommonUtilities\EInputReturn.h"
#include "..\CommonUtilities\XInputWrapper.h"
#include "..\CommonUtilities\EKeyboardKeys.h"
#include "KartControllerComponent.h"
#include "SpeedHandlerManager.h"

//Temp includes
#include "BoostData.h"

CKeyboardController::CKeyboardController(CKartControllerComponent& aKartComponent) : CController(aKartComponent)
{
	myIsMovingBackwards = false;
	myIsMovingFoward = false;
	myIsTurningLeft = false;
	myIsTurningRight = false;
}

CKeyboardController::~CKeyboardController()
{
}

CU::eInputReturn CKeyboardController::TakeInput(const CU::SInputMessage & aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eKeyboardReleased)
	{
		ReleasedKey(aInputMessage);
	}
	else if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		PressedKey(aInputMessage);
	}

	return CU::eInputReturn::ePassOn;
}

void CKeyboardController::ReleasedKey(const CU::SInputMessage & aInputMessage)
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

void CKeyboardController::PressedKey(const CU::SInputMessage & aInputMessage)
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

	case CU::eKeys::B:
	{
		SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPowerUp"));
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);
	}
	case CU::eKeys::SPACE:
	{
		/*SComponentMessageData boostMessageData;
		boostMessageData.myBoostData = CSpeedHandlerManager::GetInstance()->GetData(std::hash<std::string>()("BoostPad"));
		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost, boostMessageData);*/

		myControllerComponent.GetParent()->NotifyComponents(eComponentMessageType::eUseItem, SComponentMessageData());

		break;
	}
	case CU::eKeys::LCONTROL:
		myControllerComponent.Drift();
		break;

	case CU::eKeys::RETURN:
		myControllerComponent.GetHit();
		break;
	}
}
