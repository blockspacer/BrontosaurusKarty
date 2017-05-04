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
	//beteende för AIn sen kalla på myControllerComponents funktioner för att flytta objektet
}
