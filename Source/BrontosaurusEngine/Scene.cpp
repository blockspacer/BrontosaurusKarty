﻿#include "stdafx.h"
#include "Scene.h"
#include "ModelInstance.h"
#include "Engine.h"
#include "ModelManager.h"
#include "PointLightInstance.h"
#include "SpotLightInstance.h"
#include "Camera.h"
#include "Renderer.h"
#include "Skybox.h"
#include "Cubemap.h"
#include "ParticleEmitterInstance.h"
#include "FireEmitterInstance.h"
#include "..\CommonUtilities\Sphere.h"
#include "../Components/ParticleEmitterComponentManager.h"
#include "CascadeShadowMap.h"
#include "ParticleEmitterManager.h"

#define Intify(A_ENUM_CLASS) static_cast<int>(A_ENUM_CLASS)
#define PlayerOneCamera myRenderCameras[Intify(eCameraType::ePlayerOneCamera)]
//#define USE_SHADOWS

CScene::CScene()
{
	myModels.Init(4096);
	myPointLights.Init(32);
	mySpotLights.Init(512);
	//myParticleEmitters.Init(8);
	myFireEmitters.Init(8);
	mySkybox = nullptr;
	myCubemap = nullptr;

	myShadowMap = new CCascadeShadowMap(0, 0.1f, 150.f);
	CParticleEmitterComponentManager::GetInstance().SetScene(this);
}

CScene::~CScene()
{
	if (mySkybox && mySkybox->DecRef() <= 0)
	{
		SAFE_DELETE(mySkybox);
	}
	myModels.DeleteAll();
	//myParticleEmitters.DeleteAll();
}

void CScene::Update(const CU::Time aDeltaTime)
{
	//CParticleEmitterManager::GetInstance().Update(aDeltaTime);
	for (CModelInstance* model : myModels)
	{
		model->Update(aDeltaTime);
	}
	/*for (CParticleEmitterInstance* particle : myParticleEmitters)
	{
		if (particle == nullptr) continue;
		particle->Update(aDeltaTime);
	}*/
}

void CScene::Render()
{
	if (myCubemap)
	{
		myCubemap->SetShaderResource();
	}

	SChangeStatesMessage statemsg;
	if (mySkybox)
	{
		statemsg.myBlendState = eBlendState::eNoBlend;
		statemsg.myRasterizerState = eRasterizerState::eNoCulling;
		statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
		statemsg.mySamplerState = eSamplerState::eWrap;
		PlayerOneCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

		SRenderSkyboxMessage* msg = new SRenderSkyboxMessage();
		mySkybox->AddRef();
		msg->mySkybox = mySkybox;
		PlayerOneCamera.AddRenderMessage(msg);
	}

#ifdef USE_SHADOWS
	myShadowMap->ComputeShadowProjection(PlayerOneCamera.GetCamera());
	myShadowMap->Render(myModels);
#endif

	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	statemsg.myBlendState = eBlendState::eNoBlend;
	statemsg.mySamplerState = eSamplerState::eDeferred;
	PlayerOneCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	for (unsigned int i = 0; i < myPointLights.Size(); ++i)
	{
		if (myPointLights[i].GetIsActive() == false)
		{
			continue;
		}

		CU::Sphere lightSphere;
		lightSphere.myCenterPos = myPointLights[i].GetPosition();
		lightSphere.myRadius = myPointLights[i].GetRange();

		if (PlayerOneCamera.GetCamera().IsInside(lightSphere) == false)
			continue;

		SRenderPointLight pointlightMessage;
		pointlightMessage.pointLight = myPointLights[i].GetData();
		PlayerOneCamera.AddRenderMessage(new SRenderPointLight(pointlightMessage));
	}

	for (unsigned int i = 0; i < mySpotLights.Size(); ++i)
	{
		if (mySpotLights[i].GetIsActive() == false)
		{
			continue;
		}
		SRenderSpotLight spotlightMessage;
		spotlightMessage.spotLight = mySpotLights[i].GetData();

		CU::Sphere lightSphere;
		lightSphere.myCenterPos = spotlightMessage.spotLight.position;
		lightSphere.myRadius = spotlightMessage.spotLight.range;

		if (PlayerOneCamera.GetCamera().IsInside(lightSphere) == false)
			continue;

		PlayerOneCamera.AddRenderMessage(new SRenderSpotLight(spotlightMessage));
	}


	CParticleEmitterManager::GetInstance().Render(PlayerOneCamera);

	for (unsigned int i = 0; i < myModels.Size(); ++i)
	{
		if (myModels[i] == nullptr)
		{
			continue;
		}
	
		if (myModels[i]->GetIgnoreDepth() == false)
		{
			if (PlayerOneCamera.GetCamera().IsInside(myModels[i]->GetModelBoundingSphere()) == false)
				continue;
	
			myModels[i]->RenderDeferred(PlayerOneCamera);
		}
	}
	PlayerOneCamera.AddRenderMessage(new SRenderModelBatches());

	

	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	statemsg.myBlendState = eBlendState::eNoBlend;
	statemsg.mySamplerState = eSamplerState::eDeferred;
	PlayerOneCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	for (CModelInstance* model : myModels)
	{
		if (model == nullptr)
		{
			continue;
		}
		if (model->GetIgnoreDepth() == true)
		{
			model->RenderDeferred(PlayerOneCamera);
		}
	}
	PlayerOneCamera.AddRenderMessage(new SRenderModelBatches());

	SRenderDirectionalLight light;
	light.directionalLight = myDirectionalLight;
	PlayerOneCamera.AddRenderMessage(new SRenderDirectionalLight(light));

	statemsg.myBlendState = eBlendState::eAlphaBlend;
	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eReadOnly;
	statemsg.mySamplerState = eSamplerState::eClamp;
	PlayerOneCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	/*for (unsigned int i = 0; i < myParticleEmitters.Size(); ++i)
	{
		if (myParticleEmitters[i] == nullptr || myParticleEmitters[i]->IsVisible() == false)
		{
			continue;
		}

		myParticleEmitters[i]->Render(PlayerOneCamera);
	}*/
	//CParticleEmitterManager::GetInstance().Render(PlayerOneCamera);
	
	PlayerOneCamera.Render();

	statemsg.myBlendState = eBlendState::eAlphaBlend;
	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	statemsg.mySamplerState = eSamplerState::eClamp;
	RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	RENDERER.AddRenderMessage(new SActivateRenderToMessage());

	SRenderToIntermediate * interMSG = new SRenderToIntermediate();
	interMSG->myRect = { 0.0f, 0.0f, 1.0f, 1.0f };
	interMSG->useDepthResource = false;
	interMSG->myRenderPackage = PlayerOneCamera.GetRenderPackage();
	RENDERER.AddRenderMessage(interMSG);

	//DRAW SHADOWBUFFER
	//SRenderToIntermediate * interMSG2 = new SRenderToIntermediate();
	//interMSG2->myRect = { 0.0f, 0.0f, 0.5f, 0.5f };
	//interMSG2->useDepthResource = false;
	//interMSG2->myRenderPackage = myShadowMap->GetShadowMap();
	//RENDERER.AddRenderMessage(interMSG2);
	//RENDERER.AddRenderMessage(new SActivateRenderToMessage());
}

void CScene::RenderSplitScreen(const int aRectCount)
{
	if (aRectCount < 1 || aRectCount > 4)
	{
		DL_ASSERT("messing up rendering to split screen");
		return;
	}

	const CU::Vector4f oneSplit[2] = { CU::Vector4f(0.f, 0.f, 1.f, 0.5f), CU::Vector4f(0.f, 0.5f, 1.f, 1.f) };
	const CU::Vector4f twoSplit[4] = { CU::Vector4f(0.f, 0.f, 0.5f, 0.5f), CU::Vector4f(0.5f, 0.f, 1.f, 0.5f), CU::Vector4f(0.f, 0.5f, 0.5f, 1.f), CU::Vector4f(0.5f, 0.5f, 1.f, 1.f) };
	const CU::Vector4f noSplit(0.f, 0.f, 1.f, 1.f);

	const CU::Vector4f* splits[3] = { oneSplit, twoSplit, &noSplit };
	int split = (aRectCount - 1) / 2;
	if (aRectCount == 1)
	{
		split = 2;
	}

	for (int rect = 0; rect < aRectCount; ++rect)
	{
		RenderToRect(splits[split][rect], myPlayerCameras[rect]);
	}
}

void CScene::RenderToRect(const CU::Vector4f& aRect, CRenderCamera& aCamera)
{
	if (myCubemap)
	{
		myCubemap->SetShaderResource();
	}

	SChangeStatesMessage statemsg;
	if (mySkybox)
	{
		statemsg.myBlendState = eBlendState::eNoBlend;
		statemsg.myRasterizerState = eRasterizerState::eNoCulling;
		statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
		statemsg.mySamplerState = eSamplerState::eWrap;
		aCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

		SRenderSkyboxMessage* msg = new SRenderSkyboxMessage();
		mySkybox->AddRef();
		msg->mySkybox = mySkybox;
		aCamera.AddRenderMessage(msg);
	}

	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	statemsg.myBlendState = eBlendState::eNoBlend;
	statemsg.mySamplerState = eSamplerState::eDeferred;
	aCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	for (CPointLightInstance& pointLight : myPointLights)
	{
		if (pointLight.GetIsActive() == false)
		{
			continue;
		}

		CU::Sphere lightSphere;
		lightSphere.myCenterPos = pointLight.GetPosition();
		lightSphere.myRadius = pointLight.GetRange();

		if (aCamera.GetCamera().IsInside(lightSphere) == false)
		{
			continue;
		}

		SRenderPointLight* pointlightMessage = new SRenderPointLight();
		pointlightMessage->pointLight = pointLight.GetData();
		aCamera.AddRenderMessage(pointlightMessage);
	}

	for (CSpotLightInstance & spotLight : mySpotLights)
	{
		if (spotLight.GetIsActive() == false)
		{
			continue;
		}
		SRenderSpotLight spotlightMessage;
		spotlightMessage.spotLight = spotLight.GetData();
		spotlightMessage.rotation = spotLight.GetRotation();

		CU::Sphere lightSphere;
		lightSphere.myCenterPos = spotlightMessage.spotLight.position;
		lightSphere.myRadius = spotlightMessage.spotLight.range;

		if (aCamera.GetCamera().IsInside(lightSphere) == false)
			continue;

		aCamera.AddRenderMessage(new SRenderSpotLight(spotlightMessage));
	}

	CParticleEmitterManager::GetInstance().Render(aCamera);

	for (CModelInstance* model : myModels)
	{
		if (!model)
		{
			continue;
		}

		if (model->GetIgnoreDepth())
		{
			continue;
		}

		if (aCamera.GetCamera().IsInside(model->GetModelBoundingSphere()) == false)
		{
			continue;
		}

		model->RenderDeferred(aCamera);
	}

	aCamera.AddRenderMessage(new SRenderModelBatches());

	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	statemsg.myBlendState = eBlendState::eNoBlend;
	statemsg.mySamplerState = eSamplerState::eDeferred;
	aCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	for (CModelInstance* model : myModels)
	{
		if (model == nullptr)
		{
			continue;
		}
		if (model->GetIgnoreDepth() == true)
		{
			model->RenderDeferred(aCamera);
		}
	}
	aCamera.AddRenderMessage(new SRenderModelBatches());

	SRenderDirectionalLight* light = new SRenderDirectionalLight();
	light->directionalLight = myDirectionalLight;
	aCamera.AddRenderMessage(light);

	statemsg.myBlendState = eBlendState::eAlphaBlend;
	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eReadOnly;
	statemsg.mySamplerState = eSamplerState::eClamp;
	aCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	aCamera.Render();

	statemsg.myBlendState = eBlendState::eAlphaBlend;
	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	statemsg.mySamplerState = eSamplerState::eClamp;
	RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	RENDERER.AddRenderMessage(new SActivateRenderToMessage());

	SRenderToIntermediate * interMSG = new SRenderToIntermediate();
	interMSG->myRect = aRect;//{ 0.0f, 0.0f, 1.0f, 1.0f };
	interMSG->useDepthResource = false;
	interMSG->myRenderPackage = aCamera.GetRenderPackage();
	RENDERER.AddRenderMessage(interMSG);
}

InstanceID CScene::AddModelInstance(CModelInstance* aModelInstance)
{
	InstanceID id = 0;

	if (myFreeModels.Size() < 1)
	{
		id = myModels.Size();
		myModels.Add(aModelInstance);
		return id;
	}

	id = myFreeModels.Pop();
	myModels[id] = aModelInstance;
		
	return id;
}

InstanceID CScene::AddDirectionalLight(const Lights::SDirectionalLight & aDirectionalLight)
{
	myDirectionalLight = aDirectionalLight;
	myShadowMap->SetDirection(aDirectionalLight.direction.GetNormalized());
	return 0;
}

InstanceID CScene::AddPointLightInstance(const CPointLightInstance& aPointLight)
{
	InstanceID id = 0;

	if (myFreePointlights.Size() < 1)
	{
		id = myPointLights.Size();
		myPointLights.Add(aPointLight);
		return id;
	}

	id = myFreePointlights.Pop();
	myPointLights[id] = aPointLight;
	myPointLights[id].SetActive(true);
	return id;
}

InstanceID CScene::AddSpotLightInstance(const CSpotLightInstance& aSpotLight)
{
	InstanceID id = 0;

	if (myFreeSpotlights.Size() < 1)
	{
		id = mySpotLights.Size();
		mySpotLights.Add(aSpotLight);
		return id;
	}

	id = myFreeSpotlights.Pop();
	mySpotLights[id] = aSpotLight;
	return id;
}

//InstanceID CScene::AddParticleEmitterInstance(CParticleEmitterInstance* aParticleEmitterInstance)
//{
//	if (myFreeParticleEmitters.Size() < 1)
//	{
//		InstanceID temp = myParticleEmitters.Size();
//		myParticleEmitters.Add(aParticleEmitterInstance);
//		return temp;
//	}
//
//	InstanceID tempId = myFreeParticleEmitters.Pop();
//	myParticleEmitters[tempId] = aParticleEmitterInstance;
//
//	return  tempId;
//}

InstanceID CScene::AddFireEmitters(const CFireEmitterInstance& aFireEmitter)
{
	InstanceID id = myFireEmitters.Size();
	myFireEmitters.Add(aFireEmitter);
	return id;
}

void CScene::AddCamera(const eCameraType aCameraType)
{
	myRenderCameras[static_cast<int>(aCameraType)] = CRenderCamera(); //TODO: maybe not have add
}

void CScene::InitPlayerCameras(const int aPlayerCount)
{
	if (aPlayerCount < 1 || aPlayerCount > 4)
	{
		DL_ASSERT("messing up initing player cameras");
		return;
	}

	CU::Vector2f frameSize(WINDOW_SIZE);
	const CU::Vector2f oneSplit(1.f, 0.5f);
	const CU::Vector2f twoSplit(0.5f, 0.5f);

	const CU::Vector2f splits[2] = { oneSplit, twoSplit };
	int split = (aPlayerCount - 1) / 2;

	if (aPlayerCount > 1)
	{
		frameSize *= splits[split];
	}

	for (int player = 0; player < aPlayerCount; ++player)
	{
		myPlayerCameras.Add(CRenderCamera());
		//TODO: Find correcct fov
		myPlayerCameras.GetLast().InitPerspective(65, frameSize.x, frameSize.y, 0.1f, 500.f);
		//myPlayerCameras.GetLast().SetViewport(viewp)
	}
}

void CScene::SetSkybox(const char* aPath)
{
	if (mySkybox != nullptr)
	{
		delete mySkybox;
	}

	mySkybox = new CSkybox();
	mySkybox->Init(aPath);
}
void CScene::SetSkybox(ID3D11ShaderResourceView* aSRV)
{
	if (mySkybox != nullptr)
	{
		if (mySkybox->DecRef() <= 0)
		{
			SAFE_DELETE(mySkybox);
		}
	}

	mySkybox = new CSkybox();
	mySkybox->Init(aSRV);
}

void CScene::SetCubemap(const char* aPath)
{
	SAFE_DELETE(myCubemap);
	myCubemap = new CCubemap(aPath);
}

CModelInstance* CScene::GetModelAt(const InstanceID aModelID)
{
	if (myModels.HasIndex(aModelID))
	{
		return myModels[aModelID];
	}

	return nullptr;
}

CFireEmitterInstance& CScene::GetFireEmitter(const InstanceID aFireEmitterID)
{
	if (aFireEmitterID >= myFireEmitters.Size() || aFireEmitterID < 0)
	{
		static CFireEmitterInstance nullFireEmitter;
		return nullFireEmitter;
	}

	return myFireEmitters[aFireEmitterID];
}

CRenderCamera& CScene::GetRenderCamera(const eCameraType aCameraType)
{
	return myRenderCameras[static_cast<int>(aCameraType)];
}

//CParticleEmitterInstance* CScene::GetParticleEmitterInstance(const InstanceID aParticleEmitterID)
//{
//	return (myParticleEmitters.HasIndex(aParticleEmitterID)) ? myParticleEmitters[aParticleEmitterID] : nullptr;
//}

CPointLightInstance * CScene::GetPointLightInstance(const InstanceID aID)
{
	return (myPointLights.HasIndex(aID)) ? &myPointLights[aID] : nullptr;
}

void CScene::DeleteModelInstance(CModelInstance* anInstance)
{
	InstanceID currentId;

	if (myModels.Find(anInstance, currentId) == false)
	{
		return;
	}

	DeleteModelInstance(currentId);
}

void CScene::DeleteModelInstance(const InstanceID& anId)
{
	//if thread lock here
	CModelInstance* currentModel = myModels[anId];
	myModels[anId] = nullptr;
	myFreeModels.Push(anId);
	//if thread unlock here

	delete currentModel;
}

//void CScene::DeleteParticleEmitterInstance(const InstanceID anID)
//{
//	//if thread lock here
//	
//	//static std::mutex lockHere;
//	//lockHere.lock();
//
//	if(anID > myParticleEmitters.Size())
//	{
//		return;
//	}
//	
//	CParticleEmitterInstance* emitter = myParticleEmitters[anID];
//	myParticleEmitters[anID] = nullptr;
//	myFreeParticleEmitters.Push(anID);
//
//	//lockHere.unlock();
//	
//	//if thread unlock here
//	
//	
//	delete emitter;
//	//emitter->Destroy();
//}

void CScene::RemovePointLightInstance(const InstanceID anID)
{
	myPointLights[anID].SetActive(false);
	myFreePointlights.Push(anID);

}

void CScene::GenerateCubemap()
{
	//for now, not shure if we even need this

	return;


	//SChangeStatesMessage statemsg;
	//SSetCameraMessage cameraMsg;
	//CU::VectorOnStack<CPointLightInstance, 8> culledPointlights;

	//static float piOver2 = 3.1415926f / 2.0f;

	//CU::Matrix33f rotations[6];
	//rotations[1] = CU::Matrix33f::CreateRotateAroundY(-piOver2);
	//rotations[0] = CU::Matrix33f::CreateRotateAroundY(piOver2);
	//rotations[3] = CU::Matrix33f::CreateRotateAroundX(piOver2);
	//rotations[2] = CU::Matrix33f::CreateRotateAroundX(-piOver2);
	//rotations[4] = CU::Matrix33f::Identity;
	//rotations[5] = CU::Matrix33f::CreateRotateAroundY(PI);

	//CU::Camera camera;
	//camera.Init(90.f, width, height, 1.0f, 100.f);

	//for (int i = 0; i < 6; ++i)
	//{
	//	myCubemap->ActivateForRender(i);
	//	cameraMsg.myCamera = camera;
	//	cameraMsg.myCamera.SetTransformation(rotations[i]);
	//	RENDERER.AddRenderMessage(new SSetCameraMessage(cameraMsg));

	//	if (mySkybox)
	//	{
	//		statemsg.myBlendState = eBlendState::eNoBlend;
	//		statemsg.myRasterizerState = eRasterizerState::eNoCulling;
	//		statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	//		statemsg.mySamplerState = eSamplerState::eWrap;
	//		RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	//		SRenderSkyboxMessage* msg = new SRenderSkyboxMessage();
	//		mySkybox->AddRef();
	//		msg->mySkybox = mySkybox;
	//		RENDERER.AddRenderMessage(msg);
	//		statemsg.myRasterizerState = eRasterizerState::eDefault;
	//		statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	//		statemsg.myBlendState = eBlendState::eNoBlend;
	//		statemsg.mySamplerState = eSamplerState::eClamp;
	//		RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	//	}
	//	
	//	
	//	// mebe not neededo? - said Greedo
	//	CU::Vector3f lightDir = myDirectionalLight.direction;
	//	myDirectionalLight.direction = CU::Vector3f::Zero;
	//	//
	//	for (unsigned int j = 0; j < myModels.Size(); ++j)
	//	{
	//		if (myModels[j] == nullptr || myModels[j]->ShouldRender() == false)
	//		{
	//			continue;
	//		}
	//		myModels[j]->Render(&myDirectionalLight, culledPointlights);
	//	}
	//	myDirectionalLight.direction = lightDir;
	//}

	//RENDERER.AddRenderMessage(new SActivateRenderToMessage());
	//myCubemap->SetShaderResource();
}

CRenderCamera& CScene::GetPlayerCamera(const int aPlayerIndex)
{
	return myPlayerCameras[aPlayerIndex];
}

CSpotLightInstance* CScene::GetSpotLightInstance(const InstanceID aID)
{
	return (mySpotLights.HasIndex(aID)) ? &mySpotLights[aID] : nullptr;
}
