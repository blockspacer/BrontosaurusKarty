#include "stdafx.h"
#include "PlayerControllerManager.h"
#include "KeyboardController.h"
#include "XboxController.h"

#include "../CommonUtilities/InputMessenger.h"

CPlayerControllerManager::CPlayerControllerManager()
{
	myPlayerControllers.Init(4);
}


CPlayerControllerManager::~CPlayerControllerManager()
{
	myPlayerControllers.DeleteAll();
}

CKeyboardController* CPlayerControllerManager::CreateKeyboardController(CKartControllerComponent& aKartComponent)
{
	CKeyboardController* controller = new CKeyboardController(aKartComponent);

	CU::CInputMessenger* playState = CU::CInputMessenger::GetInstance(eInputMessengerType::ePlayState);
	if (!playState)
	{
		DL_MESSAGE_BOX("Play state input messenger cannot be found");
		return nullptr;
	}

	playState->Subscribe(*controller);

	myPlayerControllers.Add(controller);

	return controller;
}

CXboxController* CPlayerControllerManager::CreateXboxController(CKartControllerComponent& aKartComponent, short aControllerIndex)
{
	CXboxController* controller = new CXboxController(aKartComponent);

	CU::CInputMessenger* playState = CU::CInputMessenger::GetInstance(eInputMessengerType::ePlayState);
	if (!playState)
	{
		DL_MESSAGE_BOX("Play state input messenger cannot be found");
		return nullptr;
	}

	playState->Subscribe(*controller);
	controller->SetIndex(aControllerIndex);

	myPlayerControllers.Add(controller);

	return controller;
}
