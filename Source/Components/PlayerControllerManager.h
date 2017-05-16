#pragma once

class CController;
class CKeyboardController;
class CXboxController;
class CKartControllerComponent;

class CPlayerControllerManager
{
public:
	CPlayerControllerManager();
	~CPlayerControllerManager();

	CKeyboardController* CreateKeyboardController(CKartControllerComponent& aKartComponent);
	CXboxController* CreateXboxController(CKartControllerComponent& aKartComponent, short aControllerIndex);

	void Update();


private:
	CU::GrowingArray<CController*> myPlayerControllers;

};

