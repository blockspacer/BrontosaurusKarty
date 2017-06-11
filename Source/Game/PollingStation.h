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
	
	void BindKartList(CU::GrowingArray<CGameObject*>* aKartList);
	const CU::GrowingArray<CGameObject*>* GetKartList();

private:
	CPollingStation();
	~CPollingStation();

private:
	static CPollingStation* ourInstance;
	CU::GrowingArray<CGameObject*> myPlayers;
	CU::GrowingArray<CGameObject*>* myKartList;
};

