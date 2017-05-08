#pragma once

class CKartSpawnPoint;

class CKartSpawnPointManager
{
public:
	static void Create();
	static void Destroy();
	static CKartSpawnPointManager& GetInstance(); //Make friend?
	void AddSpawnPoint(CKartSpawnPoint* aSpawnPoint);
private:
	CKartSpawnPointManager();
	~CKartSpawnPointManager();
	CU::GrowingArray<CKartSpawnPoint*> mySpawnPoints;

	static CKartSpawnPointManager* ourInstance;
};

