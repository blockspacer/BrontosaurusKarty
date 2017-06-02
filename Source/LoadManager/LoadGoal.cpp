#include "stdafx.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "GoalComponent.h"
#include "KartControllerComponentManager.h"

int LoadGoal(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	if (CComponentManager::GetInstancePtr() != nullptr)
	{
		CKartControllerComponentManager* kartControllerComponentManager = loadManager.GetCurrentPLaystate().GetKartControllerComponentManager();
		CGoalComponent* goalComponent = new CGoalComponent();
		CComponentManager::GetInstancePtr()->RegisterComponent(goalComponent);
		kartControllerComponentManager->SetGoalComponent(goalComponent);
		return goalComponent->GetId();
	}

	return NULL_COMPONENT;
}
