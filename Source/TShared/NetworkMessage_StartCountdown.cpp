#include "stdafx.h"
#include "NetworkMessage_StartCountdown.h"


CNetworkMessage_StartCountdown::CNetworkMessage_StartCountdown()
{
	myHeader.myPackageType = (ePackageType::eStartCountdown);
}

CNetworkMessage_StartCountdown::~CNetworkMessage_StartCountdown()
{
}

ePackageType CNetworkMessage_StartCountdown::GetPackageType() const
{
	return ePackageType::eStartCountdown;
}

void CNetworkMessage_StartCountdown::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myCountdownTime, aStream);
}

void CNetworkMessage_StartCountdown::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myCountdownTime = deserialize<unsigned char>(aStream);
}
