#include "stdafx.h"
#include "RaceOverMessage.h"


CRaceOverMessage::CRaceOverMessage(CU::GrowingArray<CGameObject*>& aWinnerPlacementList)
	: IMessage(eMessageType::eRaceOver)
	, myWinnerPlacements(aWinnerPlacementList)
{
}


CRaceOverMessage::~CRaceOverMessage()
{
}

Postmaster::Message::IMessage * CRaceOverMessage::Copy()
{
	return new CRaceOverMessage(*this);
}

eMessageReturn CRaceOverMessage::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}