#pragma once
#include "../ThreadedPostmaster/Message.h"

class CGameObject;
class CPlayerPassedGoalMessage : public Postmaster::Message::IMessage
{
public:
	CPlayerPassedGoalMessage(CGameObject* aKartObject);
	~CPlayerPassedGoalMessage();

	IMessage *Copy() override;
	eMessageReturn DoEvent(Postmaster::ISubscriber & aSubscriber) const override;
	inline CGameObject* GetGameObject() const;

private:
	CGameObject* myKartObject;
};

inline CGameObject* CPlayerPassedGoalMessage::GetGameObject() const
{
	return myKartObject;
}
