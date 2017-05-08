#pragma once

class CKartControllerComponent;

class CController
{
public:
	CController(CKartControllerComponent& aKartComponent);
	~CController();

	virtual void Update();

protected:
	CKartControllerComponent& myControllerComponent;

};

