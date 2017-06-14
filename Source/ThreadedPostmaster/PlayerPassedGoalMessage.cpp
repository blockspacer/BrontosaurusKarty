#include "stdafx.h"
#include "PlayerPassedGoalMessage.h"

CPlayerPassedGoalMessage::CPlayerPassedGoalMessage(CGameObject* aKartObject)
	: IMessage(eMessageType::ePlayerPassedGoal)
{
	myKartObject = aKartObject;
}



CPlayerPassedGoalMessage::~CPlayerPassedGoalMessage()
{
}

Postmaster::Message::IMessage * CPlayerPassedGoalMessage::Copy()
{
	return new CPlayerPassedGoalMessage(*this);
}

eMessageReturn CPlayerPassedGoalMessage::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
