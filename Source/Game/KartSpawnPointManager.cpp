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
	if (mySpawnPoints.Size() == 0)
	{
		DL_MESSAGE_BOX("There were no (or too few) spawnpoints placed in the unity level you tried to load\nPress ignore to continue with default spawn position.");
		SKartSpawnPoint defaultSpawnPoint;
		defaultSpawnPoint.mySpawnTransformaion = CU::Matrix44f::Identity;
		return defaultSpawnPoint;
	}
	SKartSpawnPoint tempKartPoint = *mySpawnPoints.GetLast();
	delete mySpawnPoints.Pop();
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
