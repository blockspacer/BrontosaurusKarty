#include "stdafx.h"
#include "AIFinishedMessage.h"

Postmaster::ISubscriber::ISubscriber()
{
}

Postmaster::ISubscriber::~ISubscriber()
{
	Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const ConsoleCalledUpon& aConsoleCalledUpon)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const DrawCallsCount& aConsoleCalledupon)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const DroppedFile& aDroppedFile)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const FocusChange& aFocusChange)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const KeyCharPressed& aCharPressed)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const PopCurrentState& aPopCurrent)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const PushState& aPushState)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CSendNetworkMessageMessage& aSendNetowrkMessageMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const Message::CPrintMessage& aPrintMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CConectMessage& aConectMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CConectedMessage& aConectedMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CSetClientIDMessage & aSetClientIDMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CChangeLevel& aChangeLevelMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CQuitGame& aQuitGameMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CNetworkDebugInfo& aDebugInfo)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CGameEventMessage& aGameEventMessageMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const SetVibrationOnController & asetVibrationMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const StopVibrationOnController & aStopVibrationMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CPlayerFinishedMessage& aPlayerFinishedMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CAIFinishedMessage& aAIFinishedMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CRaceOverMessage& aRaceOverMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CRaceStartedMessage& aRaceOverMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CRedShellWarningMessage& aRedShellMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CBlueShellWarningMessage& aBlueShellMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const Message::CControllerInputMessage& aControllerInputMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CPlayerPassedGoalMessage& aPlayerPassedGoalMessage)
{
	return eMessageReturn::eContinue;
}

void Postmaster::ISubscriber::SetSubscribedThread(const std::thread::id & aId)
{
	if(mySubscribedId != std::thread::id() && mySubscribedId != aId)
	{
		DL_ASSERT("Postmaster subscriber is attempting to subscribe from multiple threads.");
	}

	mySubscribedId = aId;
}

const std::thread::id& Postmaster::ISubscriber::GetSubscribedId() const
{
	return mySubscribedId;
}
