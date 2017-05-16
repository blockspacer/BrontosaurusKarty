#pragma once
#include "../ThreadedPostmaster/Message.h"

class StopVibrationOnController : public Postmaster::Message::IMessage
{
public:
	StopVibrationOnController(const unsigned int aJoyStickIndex);
	~StopVibrationOnController();

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;

	const unsigned int GetJoyStick() const;
private:
	unsigned int myJoystickIndex;
};

