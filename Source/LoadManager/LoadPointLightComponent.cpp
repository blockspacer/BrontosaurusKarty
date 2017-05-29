#include "stdafx.h"
#include "LoadPointLightComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "LightComponentManager.h"
#include "GameObject.h"

int LoadPointLightComponent(KLoader::SLoadedComponentData someData)
{
	if (someData.myData.HasKey("spotAngle"))
	{
		CSpotLightComponent* spotLight = CLightComponentManager::GetInstance().CreateAndRegisterSpotLightComponent();
		spotLight->SetColor(CU::Vector3f(someData.myData.at("color").GetVector4f("xyzw") / 255.f));
		spotLight->SetIntensity(someData.myData.at("intensity").GetFloat());
		spotLight->SetRange(someData.myData.at("range").GetFloat());
		spotLight->SetSpotAngle(someData.myData.at("spotAngle").GetFloat() * (3.141692f / 180.f));

		return spotLight->GetId();
	}
	else if (someData.myData.HasKey("range"))
	{
		PointLightComponent* pointLight = CLightComponentManager::GetInstance().CreateAndRegisterPointLightComponent();
		pointLight->SetColor(CU::Vector3f(someData.myData.at("color").GetVector4f("xyzw") / 255.f));
		pointLight->SetIntensity(someData.myData.at("intensity").GetFloat());
		pointLight->SetRange(someData.myData.at("range").GetFloat());
		return pointLight->GetId();
	}
	else
	{
		int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
		CGameObject* parent = reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
		Lights::SDirectionalLight directionalLight;
		directionalLight.direction = -parent->GetToWorldTransform().myForwardVector;
		directionalLight.intensity = someData.myData.at("intensity").GetFloat();
		directionalLight.color = someData.myData.at("color").GetVector4f("xyzw") / 255.f;
		CLightComponentManager::GetInstance().AddDirectionalLightToScene(directionalLight);

		return NULL_COMPONENT;
	}
}
