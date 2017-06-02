#include "stdafx.h"
#include "Controller.h"

CController::CController(CKartControllerComponent& aKartComponent)
	: myControllerComponent(aKartComponent)
{
}


CController::~CController()
{
}

void CController::Update(const float /*aDeltaTime*/)
{
}
