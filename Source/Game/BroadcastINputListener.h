#pragma once
#include "../CommonUtilities/InputListener.h"

class CBroadcastINputListener: public CU::IInputListener
{
public:
	CBroadcastINputListener();
	~CBroadcastINputListener();
	CU::eInputReturn TakeInput(const CU::SInputMessage& aInputMessage) override;
};

