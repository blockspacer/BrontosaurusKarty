#include "stdafx.h"
#include "KartSpawnPointManager.h"

CKartSpawnPointManager* CKartSpawnPointManager::ourInstance = nullptr;

void CKartSpawnPointManager::Create()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CKartSpawnPointManager();
	}
	else
	{
		DL_ASSERT("CKartSpawnManager already created");
	}
}

void CKartSpawnPointManager::Destroy()
{
	SAFE_DELETE(ourInstance);
}

CKartSpawnPointManager* CKartSpawnPointManager::GetInstance()
{
	return ourInstance;
}

void CKartSpawnPointManager::PushSpawnPoint(SKartSpawnPoint* aSpawnPoint)
{
	mySpawnPoints.Add(aSpawnPoint);
}

SKartSpawnPoint CKartSpawnPointManager::PopSpawnPoint()
{
	if (mySpawnPoints.Size() == 0) { DL_MESSAGE_BOX("There were no (or too few) spawnpoints placed in the unity level you tried to load."); }
	SKartSpawnPoint tempKartPoint = *mySpawnPoints.GetLast();
	SAFE_DELETE(mySpawnPoints.GetLast());
	return tempKartPoint;
}

CKartSpawnPointManager::CKartSpawnPointManager()
{
	mySpawnPoints.Init(8);
}

CKartSpawnPointManager::~CKartSpawnPointManager()
{
	mySpawnPoints.DeleteAll();
}
