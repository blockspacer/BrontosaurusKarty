#pragma once
#include "../ThreadedPostmaster/Message.h"

struct SPlacementData;

class CGameObject;
class CRaceOverMessage : public Postmaster::Message::IMessage
{
public:
	CRaceOverMessage(CU::StaticArray<SPlacementData,8>& aPlacementDataList);
	~CRaceOverMessage();

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;
	inline const CU::StaticArray<SPlacementData, 8>& GetWinners() const;
private:
	CU::StaticArray<SPlacementData, 8>& myWinners;
};

inline const CU::StaticArray<SPlacementData, 8>& CRaceOverMessage::GetWinners() const
{
	return myWinners;
}