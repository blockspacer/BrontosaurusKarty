#include "stdafx.h"
#include "GetAvailableController.h"
#include "ThreadedPostmaster/Subscriber.h"

GetAvailableController::GetAvailableController() : IMessage(eMessageType::eGetAvailableController)
{
}


GetAvailableController::~GetAvailableController()
{
}

Postmaster::Message::IMessage * GetAvailableController::Copy()
{
	return new GetAvailableController(*this);
}

eMessageReturn GetAvailableController::DoEvent(::Postmaster::ISubscriber & aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
