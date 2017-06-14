#include "stdafx.h"
#include "LoadLightsEnvironment.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "LightComponentManager.h"
#include "GameObject.h"
#include "Engine.h"
#include "Renderer.h"
#include "DecalComponent.h"
#include "SailInCirclesComponent.h"
#include "SailInCirclesManager.h"

int LoadPointLightComponent(KLoader::SLoadedComponentData someData)
{
	if (someData.myData.HasKey("spotAngle"))
	{
		CSpotLightComponent* spotLight = CLightComponentManager::GetInstance().CreateAndRegisterSpotLightComponent();
		spotLight->SetColor(CU::Vector3f(someData.myData.at("color").GetVector4f("xyzw") / 255.f));
		spotLight->SetIntensity(someData.myData.at("intensity").GetFloat());
		spotLight->SetRange(someData.myData.at("range").GetFloat());
		spotLight->SetSpotAngle(someData.myData.at("spotAngle").GetFloat() * (3.141692f / 180.f));

		int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
		CGameObject* parent = reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
		CU::Matrix44f rotated = parent->GetToWorldTransform();
		rotated.RotateAroundAxis(3.141592f, CU::Axees::Y);
		parent->SetWorldTransformation(rotated);

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
		directionalLight.direction = -parent->GetToWorldTransform().myForwardVector;// *CU::Matrix44f::CreateRotateAroundY(3.141592f);
		directionalLight.intensity = someData.myData.at("intensity").GetFloat();
		directionalLight.color = someData.myData.at("color").GetVector4f("xyzw") / 255.f;
		CLightComponentManager::GetInstance().AddDirectionalLightToScene(directionalLight);

		return NULL_COMPONENT;
	}
}

int LoadEnvironmentSettings(KLoader::SLoadedComponentData someData)
{
	std::string directory = "Models/Textures/";
	std::string skybox = directory + someData.myData.at("skybox").GetString();
	std::string cubemap = directory + someData.myData.at("cubemap").GetString();

	CLightComponentManager::GetInstance().SetSkybox(skybox.c_str());
	CLightComponentManager::GetInstance().SetCubemap(cubemap.c_str());

	CEngine* engine = CEngine::GetInstance();
	assert(engine && "Could not get engine instance");
	CRenderer& renderer = engine->GetRenderer();

	float start, end, ambientIntensity;
	start = someData.myData.at("fogStart").GetFloat();
	end = someData.myData.at("fogEnd").GetFloat();
	ambientIntensity = someData.myData.at("ambientIntensity").GetFloat();
	
	renderer.SetFogStartEnd(start, end);
	renderer.SetAmbientIntensity(ambientIntensity);

	CU::Vector4f color = someData.myData.at("fogColor").GetVector4f();
	color /= 255.f;
	renderer.SetFogColor(color);

	int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
	CGameObject* parent = reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));


	CU::Vector3f shadowAABBposition = parent->GetToWorldTransform().GetPosition();// someData.myData.at("shadowAABBposition").GetVector3f();
	CU::Vector3f shadowAABBhalfWidth = someData.myData.at("shadowAABBhalfWidth").GetVector3f();
	CLightComponentManager::GetInstance().SetShadowMapAABB(shadowAABBposition, shadowAABBhalfWidth);

	return NULL_COMPONENT;
}

int LoadDecal(KLoader::SLoadedComponentData someData)
{
	CDecalComponent* component = CLightComponentManager::GetInstance().CreateAndRegisterDecalComponent();
	component->SetDecalIndex(someData.myData.at("decalIndex").GetUInt());
	return component->GetId();
}

int LoadSailInCircles(KLoader::SLoadedComponentData someData)
{
	float rpm = someData.myData.at("RPM").GetFloat();
	float rpmVert = someData.myData.at("verticalRPM").GetFloat();

	float radius = someData.myData.at("radius").GetFloat();
	float verticalAmplitude = someData.myData.at("verticalAmplitude").GetFloat();

	CComponent* comp = CSailInCirclesManager::GetInstance().CreateComponent(rpm, rpmVert, radius, verticalAmplitude);
	return comp->GetId();
}
