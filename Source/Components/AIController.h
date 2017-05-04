#pragma once
#include "Controller.h"
class CAIController : public CController
{
public:
	CAIController(CKartControllerComponent& aKartComponent);
	~CAIController();

	void Update(const float aDeltaTime) override;

private:
};

