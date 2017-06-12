#include "stdafx.h"
#include "BroadcastINputListener.h"
#include "EInputMessage.h"
#include "CommonUtilities/EInputReturn.h"


CBroadcastINputListener::CBroadcastINputListener()
{
}


CBroadcastINputListener::~CBroadcastINputListener()
{
}

CU::eInputReturn CBroadcastINputListener::TakeInput(const CU::SInputMessage& aInputMessage)
{
	return CU::eInputReturn::ePassOn;
}
