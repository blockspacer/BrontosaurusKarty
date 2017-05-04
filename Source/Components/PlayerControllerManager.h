#pragma once

class CPlayerController;
class CKartControllerComponent;

class CPlayerControllerManager
{
public:
	CPlayerControllerManager();
	~CPlayerControllerManager();

	CPlayerController* CreatePlayerController(CKartControllerComponent& aKartComponent);

	void Update();


private:
	CU::GrowingArray<CPlayerController*> myPlayerControllers;

};

