#include "stdafx.h"
#include "RaceStartedMessage.h"

eMessageReturn CRaceStartedMessage::DoEvent(Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}