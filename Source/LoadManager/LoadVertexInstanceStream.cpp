#include "stdafx.h"
#include "Components/VertexStreamComponent.h"


int LoadVertexStreamInstanceComponent(KLoader::SLoadedComponentData someData)
{
	std::string path = someData.myData.GetString();

	Component::CVertexStreamComponent* vertexStream = new Component::CVertexStreamComponent(path);

	CComponentManager::GetInstance().RegisterComponent(vertexStream);
	return vertexStream->GetId();
}
