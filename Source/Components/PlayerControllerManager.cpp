#include "stdafx.h"
#include "PlayerControllerManager.h"

#include "KeyboardController.h"
#include "XboxController.h"
#include "AIController.h"

#include "../CommonUtilities/InputMessenger.h"

#include "..\ThreadedPostmaster\Postmaster.h"
#include "..\ThreadedPostmaster\PlayerFinishedMessage.h"
#include "..\ThreadedPostmaster\MessageType.h"

#include <PollingStation.h>

CPlayerControllerManager::CPlayerControllerManager()
{
	myPlayerControllers.Init(4);
	//POSTMASTER.Subscribe(this, eMessageType::ePlayerFinished);
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

CAIController* CPlayerControllerManager::CreateAIController(CKartControllerComponent& aKartComponent)
{
	CAIController* controller = new CAIController(aKartComponent);
	myPlayerControllers.Add(controller);
	return controller;
}

void CPlayerControllerManager::Update(const float aDeltaTime)
{
	for (CController* controller : myPlayerControllers)
	{
		controller->Update(aDeltaTime);
	}
}

eMessageReturn CPlayerControllerManager::DoEvent(const CPlayerFinishedMessage& aMessage)
{
	unsigned char idOfPlayer = CPollingStation::GetInstance()->GetIDFromPlayer((CGameObject*)aMessage.GetGameObject());
	// replace controlls at ID with AI. ...assuming they're in order.
	return eMessageReturn::eContinue;
}

eMessageReturn CPlayerControllerManager::DoEvent(const CRaceStartedMessage& aMessage)
{
	int br = 0;
	// Copy controllers.
	return eMessageReturn::eContinue;
}
