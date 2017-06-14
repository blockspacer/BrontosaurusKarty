#include "stdafx.h"
#include "DeferredRenderer.h"
#include "RenderMessages.h"
#include "Engine.h"
#include "DXFramework.h"
#include "ModelManager.h"
#include "ConstBufferTemplate.h"
#include "FullScreenHelper.h"
#include "Lights.h"
#include "Renderer.h"
#include "LightModel.h"
#include "Engine.h"
#include "ShaderManager.h"
#include "FBXLoader.h"
#include "ParticleEmitterManager.h"
#include "ParticleEmitter.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Engine.h"
#include "Sprite.h"

#ifdef _ENABLE_RENDERMODES
#include <../CommonUtilities/InputWrapper.h>
#include "WindowsWindow.h"
#include "EKeyboardKeys.h"
#endif

CDeferredRenderer::CDeferredRenderer() : myGbuffer(nullptr)
{
	myBatchedModelIds.Init(64);
	myFramework = CEngine::GetInstance()->GetFramework();
	CU::Vector2ui windowSize = CEngine::GetInstance()->GetWindowSize();

	//myGbuffer.Init(windowSize, myFramework, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);

	myIntermediatePackage.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	mySSAORandomTexture = &CEngine::GetInstance()->GetTextureManager().LoadTexture("SSAORandomTexture.dds");
	mySSAOPackage.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myRenderMessages.Init(128);
	myLightMessages.Init(128);
	myDecalMessages.Init(16);

	SCameraStruct temp;
	myProjectionInverseBuffer = BSR::CreateCBuffer(&temp);

	Lights::SDirectionalLight directionalLight;
	myDirectionalLightBuffer = BSR::CreateCBuffer(&directionalLight);

	Lights::SPointLight pointLight;
	myPointLightBuffer = BSR::CreateCBuffer(&pointLight);

	Lights::SSpotLight spotLight;
	mySpotLightBuffer = BSR::CreateCBuffer(&spotLight);

	SDecalData decal;
	myDecalBuffer = BSR::CreateCBuffer(&decal);

	InitLightModels();
	InitDecalModel();

	
	my3DSprites.Init(20);

#ifdef _ENABLE_RENDERMODES
	myInputWrapper = new CU::InputWrapper();
	myInputWrapper->Init(HINSTGET, HWNDGET);
	myRenderMode = ERenderMode::eIntermediate;
#endif
}

CDeferredRenderer::~CDeferredRenderer()
{
	SAFE_RELEASE(myProjectionInverseBuffer);
	SAFE_RELEASE(myDirectionalLightBuffer);
	SAFE_RELEASE(myPointLightBuffer);
	SAFE_RELEASE(mySpotLightBuffer);
	SAFE_DELETE(myPointLightModel);
	SAFE_DELETE(mySpotLightModel);
	SAFE_DELETE(myDecalModel);

#ifdef _ENABLE_RENDERMODES
	SAFE_DELETE(myInputWrapper);
#endif
}

void CDeferredRenderer::InitLightModels()
{
	myPointLightModel = new CLightModel();
	mySpotLightModel = new CLightModel();

	CFBXLoader modelLoader;
	CLoaderModel* pointModel = modelLoader.LoadModel("Models/lightmeshes/sphere.fbx");
	CLoaderMesh* pointMesh = pointModel->myMeshes[0];

	CLoaderModel* spotModel = modelLoader.LoadModel("Models/lightmeshes/cone.fbx");
	CLoaderMesh* spotMesh = spotModel->myMeshes[0];

	unsigned int shaderBlueprint = pointMesh->myShaderType;
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);
	ID3D11PixelShader* pixelShaderPoint = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_pointlight.fx", shaderBlueprint);
	ID3D11PixelShader* pixelShaderSpot = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_spotlight.fx", shaderBlueprint);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);


	CEffect* pointEffect = new CEffect(vertexShader, pixelShaderPoint, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CEffect* spotEffect = new CEffect(vertexShader, pixelShaderSpot, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	myPointLightModel->Init(pointEffect, pointMesh);
	mySpotLightModel->Init(spotEffect, spotMesh);

	myDecalDiffuse = &CEngine::GetInstance()->GetTextureManager().LoadTexture("Models/Textures/decalDiffuse.dds");

	delete pointMesh;
	delete pointModel;

	delete spotMesh;
	delete spotModel;
}

void CDeferredRenderer::InitDecalModel()
{
	myDecalModel = new CLightModel();

	CFBXLoader modelLoader;
	CLoaderModel* decalModel = modelLoader.LoadModel("Models/lightmeshes/cube.fbx");
	CLoaderMesh* decalMesh = decalModel->myMeshes[0];

	unsigned int shaderBlueprint = decalMesh->myShaderType;
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_decal.fx", shaderBlueprint);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);


	CEffect* decalEffect = new CEffect(vertexShader, pixelShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	myDecalModel->Init(decalEffect, decalMesh);

	delete decalMesh;
	delete decalModel;
}

void CDeferredRenderer::DoRenderQueue(CRenderer& aRenderer)
{
	if (myGbuffer->IsInited() == false)
	{
		myRenderMessages.RemoveAll();
		return;
	}
	myGbuffer->Clear();	
	myGbuffer->UnbindInput();
	myGbuffer->BindOutput();

	for (unsigned int i = 0; i < myRenderMessages.Size(); ++i)
	{
		switch (myRenderMessages[i]->myType)
		{
		case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
		{
			SRenderModelDeferredMessage* msg = static_cast<SRenderModelDeferredMessage*>(myRenderMessages[i]);
			CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
			if (!modelManager) break;

			CModel* model = modelManager->GetModel(msg->myModelID);
			if (!model) break;
			model->Render(msg->myRenderParams, Render::SEffectData());
			break;
		}
		case SRenderMessage::eRenderMessageType::eRenderModelInstanced:
		{
			SRenderModelInstancedMessage* msg = static_cast<SRenderModelInstancedMessage*>(myRenderMessages[i]);
			CModel* model = CEngine::GetInstance()->GetModelManager()->GetModel(msg->myModelID);
			model->AddInstanceToRenderBuffer(msg->myRenderParams);

			if (myBatchedModelIds.Find(msg->myModelID) == myBatchedModelIds.FoundNone)
			{
				myBatchedModelIds.Add(msg->myModelID);
			}
			break;
		}
		case SRenderMessage::eRenderMessageType::eRenderModelBatches:
		{
			SRenderModelBatches* msg = static_cast<SRenderModelBatches*>(myRenderMessages[i]);
			CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
			if (!modelManager) break;

			CModel* model = nullptr;

			for (int id : myBatchedModelIds)
			{
				model = modelManager->GetModel(id);
				if (!model) continue;
				Render::SEffectData effectData;
				effectData.pixelBlueprint |= EModelBluePrint_Instance;
				effectData.vertexBlueprint |= EModelBluePrint_Instance;
				effectData.geometryBlueprint |= EModelBluePrint_Instance;
				model->RenderInstanced(msg->myRenderToDepth, msg->myPixelShader, effectData);
			}
			myBatchedModelIds.RemoveAll();
			break;
		}
		case SRenderMessage::eRenderMessageType::eChangeStates:
		{
			SChangeStatesMessage* msg = static_cast<SChangeStatesMessage*>(myRenderMessages[i]);
			aRenderer.SetStates(msg);
			break;
		}
		
		
		default:
			break;
		}
	}

	
	myRenderMessages.RemoveAll();
	myGbuffer->UnbindInput();
	myGbuffer->UnbindOutput();
}

void CDeferredRenderer::AddRenderMessage(SRenderMessage* aRenderMessage)
{
	switch (aRenderMessage->myType)
	{
	case SRenderMessage::eRenderMessageType::eRenderSpotLight:
	case SRenderMessage::eRenderMessageType::eRenderPointLight:
	case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
		myLightMessages.Add(aRenderMessage);
		break;
	case SRenderMessage::eRenderMessageType::eRenderDecal:
		myDecalMessages.Add(aRenderMessage);
		break;
	case SRenderMessage::eRenderMessageType::e3DSprite:
		{
			SRender3DSpriteMessage* message = static_cast<SRender3DSpriteMessage*>(aRenderMessage);
			my3DSprites.Add(message);
		}
		
		break;
	default:
		myRenderMessages.Add(aRenderMessage);
		break;
	}
}

void CDeferredRenderer::UpdateCameraBuffer(const CU::Matrix44f & aCameraSpace, const CU::Matrix44f & aProjectionInverse)
{
	SCameraStruct updatedMatrices;
	updatedMatrices.prjInv = aProjectionInverse;
	updatedMatrices.camTW = aCameraSpace;

	BSR::UpdateCBuffer<SCameraStruct>(myProjectionInverseBuffer, &updatedMatrices);
}

void CDeferredRenderer::DoLightingPass(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer, const CU::Matrix44f& aCameraTransform, const CU::Matrix44f& aProjection)
{
	if (myGbuffer->IsInited() == false) return;
	SetCBuffer();

	//SSAO
#ifdef _ENABLE_RENDERMODES
	if(myRenderMode == ERenderMode::eSSAO || myRenderMode == ERenderMode::eIntermediate)
#endif
	{
		SChangeStatesMessage changeStateMessage = {};
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
		changeStateMessage.myBlendState = eBlendState::eMulBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoSSAO(aFullscreenHelper);
	}
	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);

	CGeometryBuffer::EGeometryPackages packages = CGeometryBuffer::EGeometryPackage::eALL - CGeometryBuffer::EGeometryPackage::eEmissive;


	ID3D11Resource* diffuseDepth = nullptr;
	myGbuffer->GetRenderPackage(CGeometryBuffer::eDiffuse).GetDepthResource()->GetResource(&diffuseDepth);

	ID3D11Resource* emissiveDepth = nullptr;
	myGbuffer->GetRenderPackage(CGeometryBuffer::eEmissive).GetDepthResource()->GetResource(&emissiveDepth);

	DEVICE_CONTEXT->CopyResource(emissiveDepth, diffuseDepth);


	myGbuffer->BindOutput(packages, CGeometryBuffer::eEmissive);
	myGbuffer->BindInput(CGeometryBuffer::EGeometryPackage::eDepth, CGeometryBuffer::eDiffuse);


	DEVICE_CONTEXT->PSSetShaderResources(1, 1, myDecalDiffuse->GetShaderResourceViewPointer());

	DoDecals(aRenderer);
	
	myGbuffer->UnbindOutput();
	ActivateIntermediate();
	myGbuffer->BindInput();

#ifdef _ENABLE_RENDERMODES
	myInputWrapper->Update();
	HandleInput();

	switch (myRenderMode)
	{
	case ERenderMode::eDiffuse:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer->GetRenderPackage(CGeometryBuffer::eDiffuse));
		break;
	case ERenderMode::eNormal:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer->GetRenderPackage(CGeometryBuffer::eNormal));
		break;
	case ERenderMode::eRMAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer->GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eRoughness:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyR, &myGbuffer->GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eMetalness:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyG, &myGbuffer->GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyB, &myGbuffer->GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eSSAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyB, &myGbuffer->GetRenderPackage(CGeometryBuffer::eRMAO));
		break;
	case ERenderMode::eEmissive:
	case ERenderMode::eHighlight:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer->GetRenderPackage(CGeometryBuffer::eEmissive));
		break;
	case ERenderMode::eDepth:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, myGbuffer->GetRenderPackage(CGeometryBuffer::eDiffuse).GetDepthResource());
		break;
	case ERenderMode::eIntermediate:
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eNoBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoAmbientLighting(aFullscreenHelper);
		DoDirectLighting(aFullscreenHelper, aRenderer);
		break;
	default:
		break;
	}

#else
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eNoBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoAmbientLighting(aFullscreenHelper);
		DoDirectLighting(aFullscreenHelper, aRenderer);
#endif
}

ID3D11DepthStencilView* CDeferredRenderer::GetDepthStencil()
{
	return myGbuffer->GetRenderPackage(CGeometryBuffer::eDiffuse).GetDepthStencilView();
}

ID3D11ShaderResourceView* CDeferredRenderer::GetDepthResource()
{
	return myGbuffer->GetRenderPackage(CGeometryBuffer::eDiffuse).GetDepthResource();
}

CRenderPackage& CDeferredRenderer::GetFirstPackage()
{
	return myGbuffer->GetRenderPackage(CGeometryBuffer::eDiffuse);
}

CRenderPackage& CDeferredRenderer::GetSecondPackage()
{
	return myGbuffer->GetRenderPackage(CGeometryBuffer::eEmissive);
}

void CDeferredRenderer::Do3DSprites(const CU::Matrix44f& aMatrix44, const CU::Matrix44f& aProjection, const CU::Vector2f& aViewportSize, int aPlayerIndex)
{
	for(int i = 0; i < my3DSprites.Size(); ++i)
	{
		SRender3DSpriteMessage* spriteMessage = my3DSprites[i];
		if (aPlayerIndex != spriteMessage->myOwnerIndex)
		{
			const CU::Vector4f pos = spriteMessage->myPosition * aMatrix44 * aProjection;

			const CU::Vector4f screenizedPos = pos / pos.w;
			CU::Vector2f screenPos = ((CU::Vector2f(screenizedPos.x, screenizedPos.y)) + CU::Vector2f::One) / 2;
			screenPos.y = 1.f - screenPos.y;
			const CU::Vector2f windowSize = WINDOW_SIZE_F;
			const CU::Vector2f viewportscaling(aViewportSize.y / windowSize.y, aViewportSize.x / windowSize.x);
			if (pos.z > 0.f)
			{
				spriteMessage->mySprite->Render(screenPos, spriteMessage->mySize * viewportscaling * (1.f / pos.z), spriteMessage->myPivot, spriteMessage->myRotation, spriteMessage->myRect, spriteMessage->myColor);
			}
		}
		
	}
	my3DSprites.RemoveAll();
}

void CDeferredRenderer::ActivateIntermediate()
{
	myGbuffer->UnbindOutput();
	myIntermediatePackage.Clear();
	myIntermediatePackage.Activate();
}

void CDeferredRenderer::SetRMAOSRV()
{
	myFramework->GetDeviceContext()->PSSetShaderResources(3, 1, &myGbuffer->GetRenderPackage(CGeometryBuffer::eRMAO).GetResource());
}

void CDeferredRenderer::SetCBuffer()
{
	myFramework->GetDeviceContext()->PSSetConstantBuffers(1, 1, &myProjectionInverseBuffer);
}

void CDeferredRenderer::DoAmbientLighting(CFullScreenHelper& aFullscreenHelper)
{
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredAmbient);
}

void CDeferredRenderer::DoDirectLighting(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer)
{
	SChangeStatesMessage cullfrontStates;
	cullfrontStates.myRasterizerState = eRasterizerState::eCullFront;
	cullfrontStates.myDepthStencilState = eDepthStencilState::eDisableDepth;
	cullfrontStates.myBlendState = eBlendState::eAddBlend;
	cullfrontStates.mySamplerState = eSamplerState::eClamp;

	SChangeStatesMessage noCullStates;
	noCullStates.myRasterizerState = eRasterizerState::eNoCulling;
	noCullStates.myDepthStencilState = eDepthStencilState::eDisableDepth;
	noCullStates.myBlendState = eBlendState::eAddBlend;
	noCullStates.mySamplerState = eSamplerState::eClamp;

	aRenderer.SetStates(&cullfrontStates);

	for (SRenderMessage* renderMessage : myLightMessages)
	{
		switch (renderMessage->myType)
		{
		case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
			aRenderer.SetStates(&noCullStates);
			RenderDirectionalLight(renderMessage, aFullscreenHelper);
			aRenderer.SetStates(&cullfrontStates);
			break;
		case SRenderMessage::eRenderMessageType::eRenderPointLight:
			RenderPointLight(renderMessage);
			break;
		case SRenderMessage::eRenderMessageType::eRenderSpotLight:
			RenderSpotLight(renderMessage);
			break;
		}
	}
	myLightMessages.RemoveAll();
}

void CDeferredRenderer::RenderDirectionalLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper)
{
	SRenderDirectionalLight* msg = static_cast<SRenderDirectionalLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SDirectionalLight>(myDirectionalLightBuffer, &msg->directionalLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &myDirectionalLightBuffer);
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredDirectional);
}

void CDeferredRenderer::RenderPointLight(SRenderMessage* aRenderMessage)
{
	SRenderPointLight* msg = static_cast<SRenderPointLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SPointLight>(myPointLightBuffer, &msg->pointLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &myPointLightBuffer);
	CU::Matrix44f pointLightTransformation;
	pointLightTransformation.myPosition = msg->pointLight.position;

	float scale = msg->pointLight.range;
	pointLightTransformation.Scale({ scale, scale, scale });
	myPointLightModel->Render(pointLightTransformation);
}

void CDeferredRenderer::RenderSpotLight(SRenderMessage* aRenderMessage)
{
	SRenderSpotLight* msg = static_cast<SRenderSpotLight*>(aRenderMessage);

	
	CU::Matrix44f spotLightTransformation = msg->rotation;
	spotLightTransformation.myPosition = msg->spotLight.position;

	float scaleXY = (msg->spotLight.spotAngle) / (3.141592f * 0.5f);
	float scaleZ = msg->spotLight.range;
	scaleXY *= scaleZ;

	spotLightTransformation.SetScale({ scaleXY, scaleXY, scaleZ });

	float tempAngle = msg->spotLight.spotAngle;
	msg->spotLight.spotAngle = std::cos(msg->spotLight.spotAngle / 2.0f);
	BSR::UpdateCBuffer<Lights::SSpotLight>(mySpotLightBuffer, &msg->spotLight);
	msg->spotLight.spotAngle = tempAngle;
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &mySpotLightBuffer);


	mySpotLightModel->Render(spotLightTransformation);
}

void CDeferredRenderer::DoSSAO(CFullScreenHelper& aFullscreenHelper)
{
	myGbuffer->UnbindOutput();
	myGbuffer->GetRenderPackage(CGeometryBuffer::eRMAO).Activate();
	CGeometryBuffer::EGeometryPackages packages = CGeometryBuffer::EGeometryPackage::eALL - CGeometryBuffer::EGeometryPackage::eRMAO;
	myGbuffer->BindInput(packages);

	myFramework->GetDeviceContext()->PSSetShaderResources(7, 1, mySSAORandomTexture->GetShaderResourceViewPointer());
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eSSAO);
}

void CDeferredRenderer::SetGeometryBuffer(CGeometryBuffer& aGeometryBuffer)
{
	myGbuffer = &aGeometryBuffer;
}

void CDeferredRenderer::DoDecals(CRenderer& aRenderer)
{
	for (SRenderMessage* message : myDecalMessages)
	{
		SRenderDecal* msg = static_cast<SRenderDecal*>(message);
		msg->myData.worldSpace = msg->myWorldSpace.GetInverted();
		BSR::UpdateCBuffer<SDecalData>(myDecalBuffer, &msg->myData);
		myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &myDecalBuffer);
		myDecalModel->Render(msg->myWorldSpace);
	}
	myDecalMessages.RemoveAll();
}

#ifdef _ENABLE_RENDERMODES
void CDeferredRenderer::HandleInput()
{
	if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F1))
	{
		myRenderMode = ERenderMode::eIntermediate;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F9))
	{
		myRenderMode = ERenderMode::eDiffuse;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F2))
	{
		myRenderMode = ERenderMode::eNormal;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F3))
	{
		myRenderMode = ERenderMode::eRoughness;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F4))
	{
		myRenderMode = ERenderMode::eMetalness;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F5))
	{
		myRenderMode = ERenderMode::eAO;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F6))
	{
		myRenderMode = ERenderMode::eSSAO;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F7))
	{
		myRenderMode = (ERenderMode)((int)ERenderMode::eEmissive | (int)ERenderMode::eHighlight);
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F8))
	{
		myRenderMode = ERenderMode::eRMAO;
	}
	else if(myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F10))
	{
		myRenderMode = ERenderMode::eDepth;
	}

}
#endif