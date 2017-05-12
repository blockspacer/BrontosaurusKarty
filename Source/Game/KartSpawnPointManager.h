#pragma once

struct SKartSpawnPoint
{
	unsigned char myID;
	CU::Matrix44f mySpawnTransformaion;
};

class CKartSpawnPointManager
{
public:
	static void Create();
	static void Destroy();
	static CKartSpawnPointManager* GetInstance();
	void PushSpawnPoint(SKartSpawnPoint* aSpawnPoint);
	SKartSpawnPoint PopSpawnPoint();

private:
	CKartSpawnPointManager();
	~CKartSpawnPointManager();
	CU::GrowingArray<SKartSpawnPoint*> mySpawnPoints;

	static CKartSpawnPointManager* ourInstance;
};

