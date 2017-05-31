#pragma once

class CSpotLightComponent;
class PointLightComponent;
class CScene;

namespace Lights
{
	struct SDirectionalLight;
}

class CLightComponentManager
{
public:
	static void Destroy();
	static void Create(CScene& aScene);
	static CLightComponentManager& GetInstance();

	PointLightComponent* CreateAndRegisterPointLightComponent();
	CSpotLightComponent* CreateAndRegisterSpotLightComponent();

	void SetSkybox(const char* aPath);
	void SetCubemap(const char* aPath);

	void Update(const CU::Time aDeltaTime);

	void AddDirectionalLightToScene(const Lights::SDirectionalLight& aDirectionalLight);

private:
	CLightComponentManager(CScene& aScene);
	~CLightComponentManager();


private:
	static CLightComponentManager* ourInstance;
	
	CU::GrowingArray<PointLightComponent*> myPointLightComponents;
	CU::GrowingArray<CSpotLightComponent*> mySpotLightComponents;

	CScene& myScene;
};

