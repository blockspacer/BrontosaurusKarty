#include "stdafx.h"
#include "KartSpawnPointManager.h"
#include "KartSpawnPoint.h"

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

CKartSpawnPointManager& CKartSpawnPointManager::GetInstance()
{
	return *ourInstance;
}

void CKartSpawnPointManager::AddSpawnPoint(CKartSpawnPoint* aSpawnPoint)
{
	mySpawnPoints.Add(aSpawnPoint);
}

CKartSpawnPointManager::CKartSpawnPointManager()
{
	mySpawnPoints.Init(8);
}

CKartSpawnPointManager::~CKartSpawnPointManager()
{
	mySpawnPoints.DeleteAll();
}
