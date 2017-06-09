#include "stdafx.h"
#include "BlueShellWarningMessage.h"
#include "../ThreadedPostmaster/Message.h"


CBlueShellWarningMessage::CBlueShellWarningMessage(CGameObject* aKartToWarn):IMessage(eMessageType::eBlueShellWarning),myKartToWarn(aKartToWarn)
{
}


CBlueShellWarningMessage::~CBlueShellWarningMessage()
{
}

Postmaster::Message::IMessage * CBlueShellWarningMessage::Copy()
{
	return new CBlueShellWarningMessage(myKartToWarn);
}

eMessageReturn CBlueShellWarningMessage::DoEvent(Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

const CGameObject* CBlueShellWarningMessage::GetKartToWarn() const
{
	return myKartToWarn;
}
