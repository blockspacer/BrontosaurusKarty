#pragma once

class CSpotLightComponent;
class PointLightComponent;
class CDecalComponent;
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
	CComponent* CreateAndRegisterSpotLightComponent(const CU::Vector3f& aColor, const float aIntensity, const float aRange, const float aSpotAngle);

	CDecalComponent* CreateAndRegisterDecalComponent();

	void SetSkybox(const char* aPath);
	void SetCubemap(const char* aPath);

	void Update(const CU::Time aDeltaTime);

	void AddDirectionalLightToScene(const Lights::SDirectionalLight& aDirectionalLight);
	void SetShadowMapAABB(const CU::Vector3f& aCenterPosition, const CU::Vector3f& aExtents);
private:
	CLightComponentManager(CScene& aScene);
	~CLightComponentManager();


private:
	static CLightComponentManager* ourInstance;
	
	CU::GrowingArray<PointLightComponent*> myPointLightComponents;
	CU::GrowingArray<CSpotLightComponent*> mySpotLightComponents;
	CU::GrowingArray<CDecalComponent*> myDecalComponents;

	CScene& myScene;
};

