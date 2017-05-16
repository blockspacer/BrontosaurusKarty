#include "stdafx.h"
#include "LoadBezierCurve.h"

int LoadBezierCurve(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	loadManager.GetCurrentPLaystate().LoadNavigationSpline(someData.myData);
	
	return CComponentManager::GetInstance().RegisterComponent(new CComponent);
}