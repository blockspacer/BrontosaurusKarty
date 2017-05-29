#include "stdafx.h"
#include "PollingStation.h"

CPollingStation* CPollingStation::ourInstance = nullptr;

void CPollingStation::Create()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CPollingStation();
	}
	else
	{
		DL_ASSERT("CPollingStation already created");
	}
}

void CPollingStation::Destroy()
{
	SAFE_DELETE(ourInstance);
}

void CPollingStation::AddPlayer(CGameObject* aPlayer)
{
	myPlayers.Add(aPlayer);
}

CGameObject* CPollingStation::GetPlayerAtID(unsigned char aID)
{
	return myPlayers[aID];
}

CPollingStation::CPollingStation()
{
	myPlayers.Init(4);
}

CPollingStation::~CPollingStation()
{
}