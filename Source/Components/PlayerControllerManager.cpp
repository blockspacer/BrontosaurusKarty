#include "stdafx.h"
#include "PlayerControllerManager.h"
#include "PlayerController.h"


CPlayerControllerManager::CPlayerControllerManager()
{
	myPlayerControllers.Init(4);
}


CPlayerControllerManager::~CPlayerControllerManager()
{
	myPlayerControllers.DeleteAll();
}

CPlayerController * CPlayerControllerManager::CreatePlayerController(CKartControllerComponent& aKartComponent)
{
	CPlayerController* controller = new CPlayerController(aKartComponent);

	myPlayerControllers.Add(controller);

	return controller;
}
