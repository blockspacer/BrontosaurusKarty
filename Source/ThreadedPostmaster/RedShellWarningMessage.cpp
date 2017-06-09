#include "stdafx.h"
#include "RedShellWarningMessage.h"


CRedShellWarningMessage::CRedShellWarningMessage(CGameObject* aKartToWarn)
	:IMessage(eMessageType::eRedShellWarning), myKartToWarn(aKartToWarn)
{
}


CRedShellWarningMessage::~CRedShellWarningMessage()
{
}

Postmaster::Message::IMessage * CRedShellWarningMessage::Copy()
{
	return new CRedShellWarningMessage(myKartToWarn);
}

eMessageReturn CRedShellWarningMessage::DoEvent(Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

const CGameObject* CRedShellWarningMessage::GetKartToWarn() const
{
	return myKartToWarn;
}
