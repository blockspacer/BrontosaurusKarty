#pragma once

class CKartControllerComponent;

class CController
{
public:
	CController(CKartControllerComponent& aKartComponent);
	virtual ~CController();

	virtual void Update(const float aDeltaTime);

protected:
	CKartControllerComponent& myControllerComponent;
};

