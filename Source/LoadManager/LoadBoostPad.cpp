#include "stdafx.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "../Game/PlayState.h"
#include "BoostPadComponent.h"
#include "BoostPadComponentManager.h"

int LoadBoostPadComponent(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);
	CBoostPadComponentManager* boostPadComponentManager = loadManager.GetCurrentPLaystate().GetBoostPadComponentManager();
	if (boostPadComponentManager != nullptr)
	{
		CBoostPadComponent* boostPadComponent = boostPadComponentManager->CreateAndRegisterComponent();
		return boostPadComponent->GetId();
	}

	return NULL_COMPONENT;
}
