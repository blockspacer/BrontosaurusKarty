#pragma once
#include "..\ThreadedPostmaster\Message.h"
#include "..\ThreadedPostmaster\MessageType.h"


class CRaceStartedMessage : public Postmaster::Message::IMessage
{
public:
	CRaceStartedMessage() :IMessage(eMessageType::eRaceStarted) {}
	~CRaceStartedMessage() {}

	IMessage *Copy() override { return new CRaceStartedMessage(*this); }
	eMessageReturn DoEvent(Postmaster::ISubscriber& aSubscriber) const override;

};

