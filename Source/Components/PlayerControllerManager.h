#pragma once

class CController;
class CKeyboardController;
class CXboxController;
class CAIController;
class CKartControllerComponent;

class CPlayerControllerManager
{
public:
	CPlayerControllerManager();
	~CPlayerControllerManager();

	CKeyboardController* CreateKeyboardController(CKartControllerComponent& aKartComponent);
	CXboxController* CreateXboxController(CKartControllerComponent& aKartComponent, short aControllerIndex);
	CAIController* CreateAIController(CKartControllerComponent& aKartComponent);

	void Update(const float aDeltaTime);


private:
	CU::GrowingArray<CController*> myPlayerControllers;

};

