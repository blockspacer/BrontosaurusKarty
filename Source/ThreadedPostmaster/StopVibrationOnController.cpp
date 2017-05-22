#include "stdafx.h"
#include "StopVibrationOnController.h"

StopVibrationOnController::StopVibrationOnController(const unsigned int aJoyStickIndex) : IMessage(eMessageType::eSetVibration)
{
	myJoystickIndex = aJoyStickIndex;
}

StopVibrationOnController::~StopVibrationOnController()
{
}

Postmaster::Message::IMessage * StopVibrationOnController::Copy()
{
	return new StopVibrationOnController(*this);
}

eMessageReturn StopVibrationOnController::DoEvent(Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

const unsigned int StopVibrationOnController::GetJoyStick() const
{
	return myJoystickIndex;
}
