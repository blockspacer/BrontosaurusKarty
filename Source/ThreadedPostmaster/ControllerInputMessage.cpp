#include "stdafx.h"
#include "ControllerInputMessage.h"

Postmaster::Message::CControllerInputMessage::CControllerInputMessage(short aControllerIndex, const InputEventData& aEventData) : IMessage(eMessageType::eControllerInput), myControllerIndex(aControllerIndex), myEventData(aEventData)
{
}

Postmaster::Message::CControllerInputMessage::~CControllerInputMessage()
{
}

Postmaster::Message::IMessage* Postmaster::Message::CControllerInputMessage::Copy()
{
	return new CControllerInputMessage(*this);
}

eMessageReturn Postmaster::Message::CControllerInputMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

const Postmaster::Message::InputEventData& Postmaster::Message::CControllerInputMessage::GetData() const
{
	return myEventData;
}
