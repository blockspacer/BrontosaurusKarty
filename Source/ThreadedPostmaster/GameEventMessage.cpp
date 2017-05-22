#include "stdafx.h"
#include "GameEventMessage.h"


CGameEventMessage::CGameEventMessage(const std::wstring& aMessage) : IMessage(eMessageType::eGameEventMessage), myMessage(aMessage)
{
}

CGameEventMessage::~CGameEventMessage()
{
}

Postmaster::Message::IMessage* CGameEventMessage::Copy()
{
	return new CGameEventMessage(myMessage);
}

eMessageReturn CGameEventMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
