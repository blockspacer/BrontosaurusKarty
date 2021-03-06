﻿#include "stdafx.h"
#include "NetworkDebugInfo.h"

CNetworkDebugInfo::CNetworkDebugInfo(const int aDataSent, const int aRoundTripTime) : IMessage(eMessageType::eDebugInfo), myDataSent(aDataSent), myRoundTripTime(aRoundTripTime)
{
}

CNetworkDebugInfo::~CNetworkDebugInfo()
{
}

Postmaster::Message::IMessage* CNetworkDebugInfo::Copy()
{
	return new CNetworkDebugInfo(myDataSent, myRoundTripTime);
}

eMessageReturn CNetworkDebugInfo::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
