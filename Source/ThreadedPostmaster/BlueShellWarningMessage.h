#pragma once
#include"../ThreadedPostmaster/Message.h"

class CGameObject;

class CBlueShellWarningMessage: public Postmaster::Message::IMessage
{
public:
	CBlueShellWarningMessage(CGameObject* aKartToWarn);
	~CBlueShellWarningMessage();

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;

	const CGameObject* GetKartToWarn() const;

private:
	CGameObject* myKartToWarn;
};

