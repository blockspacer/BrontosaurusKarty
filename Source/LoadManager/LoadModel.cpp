#include "stdafx.h"
#include "LoadModel.h"
#include "Component.h"
#include "ModelComponentManager.h"

int LoadMeshFilter(KLoader::SLoadedComponentData someData)
{
	CModelComponent* const modelComp = CModelComponentManager::GetInstance().CreateComponent(someData.myData.at("meshPath").GetString().c_str());
	if (someData.myData.HasKey("shadowCasting"))
	{
		bool shadowCasting = someData.myData.at("shadowCasting").GetBool();
		modelComp->SetIsShadowCasting(shadowCasting);
	}
	return modelComp->GetId();
}
