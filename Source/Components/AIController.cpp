#include "stdafx.h"
#include "AIController.h"


CAIController::CAIController(CKartControllerComponent& aKartComponent) : CController(aKartComponent)
{
}


CAIController::~CAIController()
{
}

void CAIController::Update(const float aDeltaTime)
{
	//beteende f�r AIn sen kalla p� myControllerComponents funktioner f�r att flytta objektet
}
