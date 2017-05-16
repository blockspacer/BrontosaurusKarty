#include "stdafx.h"
#include "ConectedMessage.h"


CConectedMessage::CConectedMessage(const unsigned short anId): IMessage(eMessageType::eNetworkMessage), myID(anId)
{
}

CConectedMessage::~CConectedMessage()
{
}

Postmaster::Message::IMessage* CConectedMessage::Copy()
{
	return new CConectedMessage(myID);
}

eMessageReturn CConectedMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
