#include "stdafx.h"
#include "SetClientIDMessage.h"


CSetClientIDMessage::CSetClientIDMessage(CPlayerNetworkComponent& aComponent): myPlayerComponet(&aComponent), IMessage(eMessageType::eNetworkMessage)
{
}

CSetClientIDMessage::~CSetClientIDMessage()
{
}

Postmaster::Message::IMessage * CSetClientIDMessage::Copy()
{
	return new CSetClientIDMessage(*myPlayerComponet);
}

eMessageReturn CSetClientIDMessage::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

CPlayerNetworkComponent * CSetClientIDMessage::GetPlayerComponent() const
{
	return myPlayerComponet;
}
