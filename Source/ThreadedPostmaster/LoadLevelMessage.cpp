#include "stdafx.h"
#include "LoadLevelMessage.h"

CLoadLevelMessage::CLoadLevelMessage(const int aLevelIndex): IMessage(eMessageType::eNetworkMessage), myLevelIndex(aLevelIndex)
{
}

CLoadLevelMessage::~CLoadLevelMessage()
{
}

Postmaster::Message::IMessage* CLoadLevelMessage::Copy()
{
	return new CLoadLevelMessage(myLevelIndex);
}

eMessageReturn CLoadLevelMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
