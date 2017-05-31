#pragma once
#include "../ThreadedPostmaster/Subscriber.h"

class CController;
class CKeyboardController;
class CXboxController;
class CAIController;
class CKartControllerComponent;
class CPlayerFinishedMessage;
class CRaceStartedMessage;

class CPlayerControllerManager : public Postmaster::ISubscriber
{
public:
	CPlayerControllerManager();
	~CPlayerControllerManager();

	CKeyboardController* CreateKeyboardController(CKartControllerComponent& aKartComponent);
	CXboxController* CreateXboxController(CKartControllerComponent& aKartComponent, short aControllerIndex);
	CAIController* CreateAIController(CKartControllerComponent& aKartComponent);

	void Update(const float aDeltaTime);

private:
	eMessageReturn DoEvent(const CPlayerFinishedMessage& aMessage) override;
	eMessageReturn DoEvent(const CRaceStartedMessage& aMessage) override;
	
private:
	CU::GrowingArray<CController*> myPlayerControllers;

};

