#pragma once
#include "../ThreadedPostmaster/Message.h"

class CGameObject;
class CPlayerFinishedMessage : public Postmaster::Message::IMessage
{
public:
	CPlayerFinishedMessage(CGameObject* aKartObject);
	~CPlayerFinishedMessage();

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;
	inline CGameObject* GetGameObject();
private:
	CGameObject* myKartObject;
};

inline CGameObject* CPlayerFinishedMessage::GetGameObject()
{
	return myKartObject;
}