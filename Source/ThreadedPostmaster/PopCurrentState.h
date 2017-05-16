#pragma once
#include "Message.h"

class PopCurrentState : public Postmaster::Message::IMessage
{
public:
	PopCurrentState();
	~PopCurrentState();

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	IMessage* Copy() override;
};

