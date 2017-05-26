#include "stdafx.h"
#include "AIFinishedMessage.h"


CAIFinishedMessage::CAIFinishedMessage(CGameObject* aKartObject)
	:IMessage::IMessage(eMessageType::eAIFInished)
{
	myKartObject = aKartObject;
}


CAIFinishedMessage::~CAIFinishedMessage()
{
}

Postmaster::Message::IMessage * CAIFinishedMessage::Copy()
{
	return new CAIFinishedMessage(*this);
}

eMessageReturn CAIFinishedMessage::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
