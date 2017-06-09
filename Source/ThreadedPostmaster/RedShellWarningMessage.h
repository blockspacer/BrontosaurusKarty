#pragma once
#include"../ThreadedPostmaster/Message.h"

class CGameObject;

class CRedShellWarningMessage : public Postmaster::Message::IMessage
{
public:
	CRedShellWarningMessage(CGameObject* aKartToWarn);
	~CRedShellWarningMessage();


	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;

	const CGameObject* GetKartToWarn() const;

private:
	CGameObject* myKartToWarn;
};

