#pragma once
#include "ImportantNetworkMessage.h"

class CNetworkMessage_StartCountdown : public CImportantNetworkMessage
{
public:
	friend class CMessageStorage;
	friend CNetworkMessage;

	CNetworkMessage_StartCountdown();
	~CNetworkMessage_StartCountdown();

	ePackageType GetPackageType() const override;

	unsigned char myCountdownTime;
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};
