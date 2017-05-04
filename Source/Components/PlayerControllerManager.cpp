#include "stdafx.h"
#include "PlayerControllerManager.h"
#include "PlayerController.h"


CPlayerControllerManager::CPlayerControllerManager()
{
	myPlayerControllers.Init(4);
}


CPlayerControllerManager::~CPlayerControllerManager()
{
}

CPlayerController * CPlayerControllerManager::CreatePlayerController(CKartControllerComponent& aKartComponent)
{
	CPlayerController* controller = new CPlayerController(aKartComponent);

	myPlayerControllers.Add(controller);

	return controller;
}

void CPlayerControllerManager::Update()
{
	for (int i = 0; i < myPlayerControllers.Size(); i++)
	{
		myPlayerControllers[i]->Update();
	}
}
