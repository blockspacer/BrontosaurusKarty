#include "stdafx.h"
#include "PlayerController.h"


CPlayerController::CPlayerController(CKartControllerComponent & aKartComponent) : CController(aKartComponent)
{
}

CPlayerController::~CPlayerController()
{
}

void CPlayerController::Update()
{
	//kolla input och kalla p� functioner hos KartControllerComponent
}
