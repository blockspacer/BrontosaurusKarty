#include "stdafx.h"
#include "SetVibrationOnController.h"

//intensity range(0-100)
SetVibrationOnController::SetVibrationOnController(const unsigned int aJoyStickIndex, const float aLeftIntensity, const float aRightIntensity, const float aTimeToRumble, const bool aShouldRumbleForever) : IMessage(eMessageType::eSetVibration)
{
	myTimeToRumble = aTimeToRumble;
	myLeftIntensity = (aLeftIntensity / 100.f) * 65000;
	myRightIntensity = (aRightIntensity / 100.f) * 65000;
	myShouldRumbleForever = aShouldRumbleForever;
	myJoystickIndex = aJoyStickIndex;
}


SetVibrationOnController::~SetVibrationOnController()
{
}

const float SetVibrationOnController::GetMyTimeToRumble() const
{
	return myTimeToRumble;
}

const unsigned int SetVibrationOnController::GetJoystickIndex() const
{
	return myJoystickIndex;
}

const unsigned short SetVibrationOnController::GetRightIntensity() const
{
	return myRightIntensity;
}

const bool SetVibrationOnController::GetShoudlRumbleForever() const
{
	return myShouldRumbleForever;
}

const unsigned short SetVibrationOnController::GetLeftIntensity() const
{
	return myLeftIntensity;
}

Postmaster::Message::IMessage * SetVibrationOnController::Copy()
{
	return new SetVibrationOnController(*this);
}

eMessageReturn SetVibrationOnController::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
