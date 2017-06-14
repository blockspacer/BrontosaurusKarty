#include "stdafx.h"
#include "RaceOverMessage.h"


CRaceOverMessage::CRaceOverMessage(CU::StaticArray<SPlacementData, 8>& aWinnerPlacementList)
	: IMessage(eMessageType::eRaceOver)
{
	for(unsigned short i = 0; i < aWinnerPlacementList.Size(); i++)
	{
		myWinners[i].character = aWinnerPlacementList[i].character;
		myWinners[i].isPlayer = aWinnerPlacementList[i].isPlayer;
		myWinners[i].placement = aWinnerPlacementList[i].placement;
		myWinners[i].minutesPassed = aWinnerPlacementList[i].minutesPassed;
		myWinners[i].secondsPassed = aWinnerPlacementList[i].secondsPassed;
		myWinners[i].hundredthsSecondsPassed = aWinnerPlacementList[i].hundredthsSecondsPassed;
		myWinners[i].inputDevice = aWinnerPlacementList[i].inputDevice;
	}
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