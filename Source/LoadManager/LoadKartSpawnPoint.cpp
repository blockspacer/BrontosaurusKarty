#include "stdafx.h"
#include "LoadKartSpawnPoint.h"
#include "KevinLoader/KevinLoader.h"
#include "ComponentManager.h"
#include "GameObject.h"
#include "LoadManager.h"
#include "..\Game\KartSpawnPointManager.h"
#include "..\CommonUtilities\DL_Assert.h"

static int id = -1; // not even sure whether this is neccessary.
// just returning some arbitrary id since this is not a component.

int LoadKartSpawnPoint(KLoader::SLoadedComponentData aSomeData)
{
	SKartSpawnPoint* spawnPoint= new SKartSpawnPoint;
	CU::Matrix44f transformation;

	CU::Vector3f tempPos = aSomeData.myData.at("position").GetVector3f();
	tempPos.z *= -1;
	transformation.SetPosition(tempPos);
	transformation.Rotate(aSomeData.myData.at("rotation").GetVector3f());
	spawnPoint->mySpawnTransformaion = transformation;

	CKartSpawnPointManager::GetInstance()->PushSpawnPoint(spawnPoint);

	id += 1;

	return id;
}