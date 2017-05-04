#include "stdafx.h"
#include "LoadKartSpawnPoint.h"
#include "KevinLoader/KevinLoader.h"
#include "ComponentManager.h"
#include "GameObject.h"
#include "LoadManager.h"
#include "..\Game\KartSpawnPointManager.h"
#include "..\Game\KartSpawnPoint.h"
#include "..\CommonUtilities\DL_Assert.h"

static int id = -1; // not even sure whether this is neccessary.
// just returning some arbitrary id since this is not a component.

int LoadKartSpawnPoint(KLoader::SLoadedComponentData aSomeData)
{
	CKartSpawnPoint* spawnPoint= new CKartSpawnPoint;
	CU::Matrix44f transformation;

	transformation.SetPosition(aSomeData.myData.at("position").GetVector3f());
	transformation.Rotate(aSomeData.myData.at("rotation").GetVector3f());
	spawnPoint->SetTransformation(transformation);

	CKartSpawnPointManager::GetInstance().AddSpawnPoint(spawnPoint);

	id += 1;

	return id;
}