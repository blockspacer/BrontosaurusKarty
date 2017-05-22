#pragma once
#include "../ThreadedPostmaster/Message.h"

class SetVibrationOnController : public Postmaster::Message::IMessage
{
public:
	SetVibrationOnController(const unsigned int aJoyStickIndex, const float aLeftIntensity, const float aRightIntensity, const float aTimeToRumble = 0, const bool aShouldRumbleForever = true);
	~SetVibrationOnController();

	const float GetMyTimeToRumble() const;
	const unsigned int GetJoystickIndex() const;
	const unsigned short GetLeftIntensity() const;
	const unsigned short GetRightIntensity() const;
	const bool GetShoudlRumbleForever() const;

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;
private:
	float myTimeToRumble;
	unsigned int myJoystickIndex;
	unsigned short myLeftIntensity;
	unsigned short myRightIntensity;
	bool myShouldRumbleForever;
};

