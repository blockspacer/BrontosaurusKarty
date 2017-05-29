#include "stdafx.h"
#include "LightComponentManager.h"
#include "PointLightComponent.h"
#include "Scene.h"
#include "SpotLightComponent.h"

CLightComponentManager* CLightComponentManager::ourInstance = nullptr;

void CLightComponentManager::Destroy()
{
	SAFE_DELETE(ourInstance);
}

void CLightComponentManager::Create(CScene& aScene)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CLightComponentManager(aScene);
	}
	else
	{
		assert(false);
	}
}

CLightComponentManager& CLightComponentManager::GetInstance()
{
	return *ourInstance;
}

PointLightComponent* CLightComponentManager::CreateAndRegisterPointLightComponent()
{
	PointLightComponent* pointLight = new PointLightComponent(myScene);
	CComponentManager::GetInstance().RegisterComponent(pointLight);
	myPointLightComponents.Add(pointLight);
	return pointLight;
}

CSpotLightComponent * CLightComponentManager::CreateAndRegisterSpotLightComponent()
{
	CSpotLightComponent* spotLight = new CSpotLightComponent(myScene);
	CComponentManager::GetInstance().RegisterComponent(spotLight);
	mySpotLightComponents.Add(spotLight);
	return spotLight;
}

void CLightComponentManager::Update(const CU::Time aDeltaTime)
{
	for (PointLightComponent* component : myPointLightComponents)
	{
		component->Update(aDeltaTime);
	}
}

void CLightComponentManager::AddDirectionalLightToScene(const Lights::SDirectionalLight& aDirectionalLight)
{
	myScene.AddDirectionalLight(aDirectionalLight);
}


CLightComponentManager::CLightComponentManager(CScene& aScene)
	: myScene(aScene)
	, myPointLightComponents(10)
	, mySpotLightComponents(10)

{
}

CLightComponentManager::~CLightComponentManager()
{
}


void CLightComponentManager::SetSkybox(const char * aPath)
{
	myScene.SetSkybox(aPath);
}

void CLightComponentManager::SetCubemap(const char * aPath)
{
	myScene.SetCubemap(aPath);
}

