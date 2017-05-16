#pragma once
#include "Message.h"

class CGameEventMessage: public Postmaster::Message::IMessage
{
public:
	CGameEventMessage(const std::wstring& aMessage);
	~CGameEventMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const std::wstring myMessage;	
};
