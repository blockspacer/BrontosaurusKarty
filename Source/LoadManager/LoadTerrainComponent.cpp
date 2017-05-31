#include "stdafx.h"
#include "Components/VertexStreamComponent.h"
#include "TerrainComponent.h"


int LoadTerrainComponent(KLoader::SLoadedComponentData someData)
{
	const float speedModifier = someData.myData["speedModifier"].GetFloat();

	Component::CTerrainComponent* terrainComponent = new Component::CTerrainComponent(speedModifier);

	CComponentManager::GetInstance().RegisterComponent(terrainComponent);
	return terrainComponent->GetId();
}
