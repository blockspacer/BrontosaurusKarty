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

unsigned char CPollingStation::GetIDFromPlayer(CGameObject * aPlayer)
{
	return myPlayers.Find(aPlayer);
}

void CPollingStation::BindKartList(CU::GrowingArray<CGameObject*>* aKartList)
{
	myKartList = aKartList;
}

const CU::GrowingArray<CGameObject*>* CPollingStation::GetKartList()
{
	return myKartList;
}

const unsigned short CPollingStation::GetAmmountOfPlayer()
{
	return myPlayers.Size();
}

CPollingStation::CPollingStation()
{
	myPlayers.Init(4);
}

CPollingStation::~CPollingStation()
{
}