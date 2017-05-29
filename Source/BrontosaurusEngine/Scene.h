#pragma once

#include <Stack.h>

#include "Lights.h" //directional //make forward declaration
#include "InstanceID.h"
#include "Camera.h"
#include "RenderCamera.h"

class CCubemap;
class CSkybox;
class CModelInstance;
class CPointLightInstance;
class CParticleEmitterInstance;
class CFireEmitterInstance;
class CCascadeShadowMap;

namespace CU
{
	class Camera;
	class Time;
}


class CScene
{
public:
	enum class eCameraType
	{
		ePlayerOneCamera,
		eGUICamera,
		eShadowCamera,
		eWeaponCamera,
		eLength,
	};

	CScene();
	~CScene();

	void Update(const CU::Time aDeltaTime);
	void Render();
	void RenderSplitScreen(const int aRectCount);
	void RenderToRect(const CU::Vector4f& aRect, CRenderCamera& aCamera);

	//Add shiz, camera, modeller ljus, partiklar
	InstanceID AddModelInstance(CModelInstance* aModelInstance); //return int
	InstanceID AddDirectionalLight(const Lights::SDirectionalLight& aDirectionalLight);
	InstanceID AddPointLightInstance(const CPointLightInstance& aPointLight);
	//InstanceID AddParticleEmitterInstance(CParticleEmitterInstance* aParticleEmitterInstance);
	InstanceID AddFireEmitters(const CFireEmitterInstance& aFireEmitter);
	void AddCamera(const eCameraType aCameraType);
	void InitPlayerCameras(const int aPlayerCount);

	void SetSkybox(const char* aPath);
	void SetSkybox(ID3D11ShaderResourceView* aSRV);

	void SetCubemap(const char* aPath);

	CModelInstance* GetModelAt(const InstanceID aModelID);
	CFireEmitterInstance& GetFireEmitter(const InstanceID aFireEmitterID);
	CRenderCamera& GetRenderCamera(const eCameraType aCameraType);
	//CParticleEmitterInstance* GetParticleEmitterInstance(const InstanceID aParticleEmitterID);
	CPointLightInstance* GetPointLightInstance(const InstanceID aID);
	
	//Delete Shiz here
	void DeleteModelInstance(CModelInstance* anInstance);
	void DeleteModelInstance(const InstanceID& anId);

	inline unsigned int GetModelListSize()   {return myModels.Size();}
	inline unsigned int GetLightListSize()   {return myPointLights.Size();}
	//inline unsigned int GetEmitterListSize() {return myParticleEmitters.Size();}
	inline unsigned int GetCameraListSize()  {return myRenderCameras.Size();}

	//void DeleteParticleEmitterInstance(const InstanceID anID);
	void RemovePointLightInstance(const InstanceID anID);

	void GenerateCubemap();

	inline CSkybox& GetSkybox();
	
	CRenderCamera& GetPlayerCamera(const int aPlayerIndex);

private:
	//Inget bör vara pekare sen.
	CCascadeShadowMap* myShadowMap;

	CU::GrowingArray<CModelInstance*, InstanceID> myModels;
	CU::Stack<InstanceID, InstanceID> myFreeModels;

	CU::GrowingArray<CPointLightInstance, InstanceID> myPointLights;
	CU::Stack<InstanceID, InstanceID> myFreePointlights;

	CU::GrowingArray<CFireEmitterInstance, InstanceID> myFireEmitters;

	CU::StaticArray<CRenderCamera, static_cast<int>(eCameraType::eLength)> myRenderCameras;
	CU::VectorOnStack<CRenderCamera, 4> myPlayerCameras;
	Lights::SDirectionalLight myDirectionalLight; //make array

	CSkybox* mySkybox;
	CCubemap* myCubemap;

	float myFogStart;
	float myFogEnd;

	//CRenderCamera myShadowCamera;
};

inline CSkybox& CScene::GetSkybox()
{
	assert(mySkybox != nullptr && "Scene skybox is nullo!");
	return *mySkybox;
}
