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
