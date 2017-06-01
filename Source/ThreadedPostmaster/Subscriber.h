#pragma once
#include "EMessageReturn.h"

class PushState;
class PopCurrentState;
class KeyCharPressed;
class FocusChange;
class DroppedFile;
class ConsoleCalledUpon;
class DrawCallsCount;
class CSendNetworkMessageMessage;
class CConectMessage;
class CConectedMessage;
class CLoadLevelMessage;
class CSetClientIDMessage;
class CChangeLevel;
class CQuitGame;
class CNetworkDebugInfo;
class CGameEventMessage;
class SetVibrationOnController;
class StopVibrationOnController;
class GetAvailableController;
class CPlayerFinishedMessage;
class CAIFinishedMessage;
class CRaceOverMessage;
class CRaceStartedMessage;

namespace Postmaster
{
	namespace Message
	{
		class CPrintMessage;
	}

	class ISubscriber
	{
	public:
		ISubscriber();
		virtual ~ISubscriber();
		virtual eMessageReturn DoEvent(const ConsoleCalledUpon& aConsoleCalledUpon);
		virtual eMessageReturn DoEvent(const DrawCallsCount& aConsoleCalledupon);
		virtual eMessageReturn DoEvent(const DroppedFile& aDroppedFile);
		virtual eMessageReturn DoEvent(const FocusChange& aFocusChange);
		virtual eMessageReturn DoEvent(const KeyCharPressed& aCharPressed);
		virtual eMessageReturn DoEvent(const PopCurrentState& aPopCurrent);
		virtual eMessageReturn DoEvent(const PushState& aPushState);
		virtual eMessageReturn DoEvent(const CSendNetworkMessageMessage& aSendNetowrkMessageMessage);
		virtual eMessageReturn DoEvent(const Message::CPrintMessage& aPrintMessage);
		virtual eMessageReturn DoEvent(const CConectMessage& aConectMessage);
		virtual eMessageReturn DoEvent(const CConectedMessage& aConectedMessage);
		virtual eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage);
		virtual eMessageReturn DoEvent(const CSetClientIDMessage& aSetClientIDMessage);
		virtual eMessageReturn DoEvent(const CChangeLevel& aChangeLevelMessage);
		virtual eMessageReturn DoEvent(const CQuitGame& aQuitGameMessage);
		virtual eMessageReturn DoEvent(const CNetworkDebugInfo& aDebugInfo);
		virtual eMessageReturn DoEvent(const CGameEventMessage & aGameEventMessageMessage);
		virtual eMessageReturn DoEvent(const SetVibrationOnController& asetVibrationMessage);
		virtual eMessageReturn DoEvent(const StopVibrationOnController& aStopVibrationMessage);
		virtual eMessageReturn DoEvent(const CPlayerFinishedMessage& aPlayerFinishedMessage);
		virtual eMessageReturn DoEvent(const CAIFinishedMessage& aAIFinishedMessage);
		virtual eMessageReturn DoEvent(const CRaceOverMessage& aRaceOverMessage);
		virtual eMessageReturn DoEvent(const CRaceStartedMessage& aRaceOverMessage);

		void SetSubscribedThread(const std::thread::id& aId);
		const std::thread::id& GetSubscribedId() const;
	private:
		std::thread::id mySubscribedId;
	};
}
