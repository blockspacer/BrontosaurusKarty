#include "stdafx.h"
#include "PlayerFinishedMessage.h"


CPlayerFinishedMessage::CPlayerFinishedMessage(CGameObject* aKartObject) 
	: IMessage(eMessageType::ePlayerFinished)
{
	myKartObject = aKartObject;
}


CPlayerFinishedMessage::~CPlayerFinishedMessage()
{
}

Postmaster::Message::IMessage * CPlayerFinishedMessage::Copy()
{
	return new CPlayerFinishedMessage(*this);
}

eMessageReturn CPlayerFinishedMessage::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
