#pragma once
#include "../ThreadedPostmaster/Message.h"

class CGameObject;
class CAIFinishedMessage : public Postmaster::Message::IMessage
{
public:
	CAIFinishedMessage(CGameObject* aKartObject);
	~CAIFinishedMessage();

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;
	inline const CGameObject* GetGameObject() const;
private:
	CGameObject* myKartObject;
};

inline const CGameObject* CAIFinishedMessage::GetGameObject() const
{
	return myKartObject;
}