#pragma once
#include "../ThreadedPostmaster/Message.h"

class CPlayerController;

class GetAvailableController : public Postmaster::Message::IMessage
{
public:
	GetAvailableController();
	~GetAvailableController();

	// Inherited via IMessage
	IMessage * Copy() override;
	eMessageReturn DoEvent(::Postmaster::ISubscriber & aSubscriber) const override;
private:

};

