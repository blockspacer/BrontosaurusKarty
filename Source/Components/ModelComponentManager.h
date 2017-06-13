#pragma once
#include "ModelComponent.h"
#include "C3DSpriteComponent.h"

#define MODELCOMP_MGR CModelComponentManager::GetInstance()

class CScene;

class CModelComponentManager
{
public:
	static CModelComponentManager& GetInstance();

	CModelComponentManager(CScene& aScene);
	~CModelComponentManager();

	CModelComponent* CreateComponent(const std::string& aModelPath);
	void DeleteModelComponent(CModelComponent* aComponent);
	C3DSpriteComponent* CreateSprite(const std::string& aCs);

private:
	static CModelComponentManager* ourInstance;

	CU::GrowingArray<CModelComponent*> myModels;
	CScene& myScene;
};

