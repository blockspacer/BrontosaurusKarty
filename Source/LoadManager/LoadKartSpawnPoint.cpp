#include "stdafx.h"
#include "LoadKartSpawnPoint.h"
#include "KevinLoader/KevinLoader.h"
#include "ComponentManager.h"
#include "GameObject.h"
#include "LoadManager.h"
#include "..\Game\KartSpawnPointManager.h"
#include "..\Game\KartSpawnPoint.h"
#include "..\CommonUtilities\DL_Assert.h"

int LoadKartSpawnPoint(KLoader::SLoadedComponentData aSomeData)
{
	CKartSpawnPoint* spawnPoint; 

	if (aSomeData.myData.HasKey("Data"))
	{
		spawnPoint = new CKartSpawnPoint;
		CU::Matrix44f transformation;
		transformation.SetPosition(aSomeData.myData.at("position").GetVector3f());
		transformation.Rotate(aSomeData.myData.at("rotation").GetVector3f());
		spawnPoint->SetTransformation(transformation);
	}

	if (spawnPoint != nullptr)
		CKartSpawnPointManager::GetInstance().AddSpawnPoint(spawnPoint);
	else
		DL_ASSERT("SpawnPoint Couldn't load key from json");

	return 0; // huh int va varför ?
}