#pragma once
#include "../ThreadedPostmaster/Message.h"

class CGameObject;
class CRaceOverMessage : public Postmaster::Message::IMessage
{
public:
	CRaceOverMessage(CU::GrowingArray<CGameObject*>& aWinnerPlacementList);
	~CRaceOverMessage();

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;
	inline const CU::GrowingArray<CGameObject*>& GetWinnerPlacements() const;
private:
	CU::GrowingArray<CGameObject*>& myWinnerPlacements;
};

inline const CU::GrowingArray<CGameObject*>& CRaceOverMessage::GetWinnerPlacements() const
{
	return myWinnerPlacements;
}