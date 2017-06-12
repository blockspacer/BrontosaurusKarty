#include "stdafx.h"
#include "ExplosionComponentManager.h"
#include "ExplosionComponent.h"


CExplosionComponentManager::CExplosionComponentManager()
{
	myComponents.Init(10);
}


CExplosionComponentManager::~CExplosionComponentManager()
{
}

CExplosionComponent * CExplosionComponentManager::CreateExplosionComponent()
{
	CExplosionComponent* explosion = new CExplosionComponent;
	CComponentManager::GetInstance().RegisterComponent(explosion);
	myComponents.Add(explosion);
	return explosion;
}

void CExplosionComponentManager::Update(const float aDeltaTime)
{
	for (int i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}
