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
	inline const CGameObject* GetGameObject() const ;
	inline const unsigned char GetPlayerID() const;
private:
	CGameObject* myKartObject;
	unsigned char myPlayerID;
};

inline const CGameObject* CPlayerFinishedMessage::GetGameObject() const 
{
	return myKartObject;
}

inline const unsigned char CPlayerFinishedMessage::GetPlayerID() const
{
	return myPlayerID;
}