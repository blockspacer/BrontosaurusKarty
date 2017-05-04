#pragma once
#include "Controller.h"
class CPlayerController : public CController
{
public:
	CPlayerController(CKartControllerComponent& aKartComponent);
	~CPlayerController();

	void Update() override;

private:
};

