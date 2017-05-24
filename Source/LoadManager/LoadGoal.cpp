#include "stdafx.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "GoalComponent.h"

int LoadGoal(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	if (CComponentManager::GetInstancePtr() != nullptr)
	{
		CGoalComponent* goalComponent = new CGoalComponent();
		CComponentManager::GetInstancePtr()->RegisterComponent(goalComponent);
		return goalComponent->GetId();
	}

	return NULL_COMPONENT;
}
