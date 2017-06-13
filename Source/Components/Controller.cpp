#include "stdafx.h"
#include "Controller.h"

#include "AIController.h"

CController::CController(CKartControllerComponent& aKartComponent)
	: myControllerComponent(aKartComponent)
{
	myAIController = nullptr;
}


CController::~CController()
{
	SAFE_DELETE(myAIController);
}

void CController::Update(const float aDeltaTime)
{
	if (myControllerComponent.GetIsControlledByAI())
	{
		if (myAIController != nullptr)
		{
			myAIController->UpdateWithoutItems(aDeltaTime);
		}
	}
}
