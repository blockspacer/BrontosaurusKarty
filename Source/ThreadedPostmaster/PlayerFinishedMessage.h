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
	inline CGameObject* GetGameObject() const ;
	inline const unsigned char GetPlayerID() const;
private:
	CGameObject* myKartObject;
	unsigned char myPlayerID;
};

inline CGameObject* CPlayerFinishedMessage::GetGameObject() const 
{
	return myKartObject;
}

inline const unsigned char CPlayerFinishedMessage::GetPlayerID() const
{
	return myPlayerID;
}