#pragma once

class CPollingStation
{
public:
	static void Create();
	static void Destroy();
	static CPollingStation* GetInstance()			{ return ourInstance; }

	void AddPlayer(CGameObject* aPlayer);
	CGameObject* GetPlayerAtID(unsigned char aID);
	unsigned char GetIDFromPlayer(CGameObject* aPlayer);

private:
	CPollingStation();
	~CPollingStation();

private:
	static CPollingStation* ourInstance;
	CU::GrowingArray<CGameObject*> myPlayers;
};

