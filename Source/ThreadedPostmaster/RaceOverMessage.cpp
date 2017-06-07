#include "stdafx.h"
#include "RaceOverMessage.h"
#include "..\game\PlacementData.h"


CRaceOverMessage::CRaceOverMessage(CU::StaticArray<SPlacementData, 8>& aWinnerPlacementList)
	: IMessage(eMessageType::eRaceOver)
	, myWinners(aWinnerPlacementList)
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