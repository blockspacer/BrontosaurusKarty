#include "stdafx.h"
#include "ShadowMap.h"
#include "ModelInstance.h"
#include "RenderMessages.h"
#include "RenderStates.h"
#include "Engine.h"
#include "Renderer.h"


CShadowMap::CShadowMap(unsigned int aShadowMapSize)
{
	myHasBaked = false;
	myData.shadowMapSize = aShadowMapSize;
	myData.pcfPasses = 4;
	myRenderCamera.InitOrthographic(10.f, 10.f, 10.0f, 1.f, aShadowMapSize, aShadowMapSize, nullptr, DXGI_FORMAT_R32_FLOAT);
	myRenderCamera.ShadowInit();
}

CShadowMap::~CShadowMap()
{
}

void CShadowMap::Render(const CU::GrowingArray<CModelInstance*, InstanceID>& aModelList)
{
	CalculateFrustum();

	SChangeStatesMessage statemsg;

	statemsg.myRasterizerState = eRasterizerState::eCullFront;
	statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	statemsg.myBlendState = eBlendState::eNoBlend;
	statemsg.mySamplerState = eSamplerState::eDeferred;
	myRenderCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	for (CModelInstance* modelInstance : aModelList)
	{
		if (modelInstance == nullptr)
			continue;

		if (modelInstance->GetIsShadowCasting() == false)
			continue;

		modelInstance->RenderDeferred(myRenderCamera);
	}

	SRenderModelBatches batchMessage;
	batchMessage.myPixelShader = myRenderCamera.GetShadowShader(true);
	batchMessage.myRenderToDepth = true;
	myRenderCamera.AddRenderMessage(new SRenderModelBatches(batchMessage));
	myRenderCamera.Render(true);

	RENDERER.AddRenderMessage(new SActivateRenderToMessage(), true);


	SRenderCallback * callback = new SRenderCallback;
	auto func = [this]()
	{
		FinishedBake();
	};

	callback->myFunction = func;
	RENDERER.AddRenderMessage(callback, true);
}

void CShadowMap::SetBoundingBox(const CU::Vector3f& aCenterPosition, const CU::Vector3f& aExtents)
{
	myBoundingBox.myCenterPos = aCenterPosition;
	myBoundingBox.myMinPos = aCenterPosition - aExtents;
	myBoundingBox.myMaxPos = aCenterPosition + aExtents;
}

void CShadowMap::SetDirection(const CU::Vector3f& aLightDirection)
{
	myLightDirection = aLightDirection;
}

void CShadowMap::SendToRenderer()
{
	SSetShadowBuffer *shadowMSG = new SSetShadowBuffer();
	shadowMSG->myShadowBufferData = myData;
	shadowMSG->myShadowBuffer = myRenderCamera.GetRenderPackage();

	RENDERER.AddRenderMessage(shadowMSG);
	RENDERER.AddRenderMessage(new SActivateRenderToMessage());
}

CRenderPackage & CShadowMap::GetShadowBuffer()
{
	return myRenderCamera.GetRenderPackage();
}

void CShadowMap::CalculateFrustum()
{
	CU::Matrix44f lightSpace;
	CU::Vector3f shadowCameraOffset = myBoundingBox.myCenterPos + (-myLightDirection * 10);
	lightSpace.SetPosition(shadowCameraOffset);

	lightSpace.LookAt(myBoundingBox.myCenterPos);
	lightSpace.SetPosition(myBoundingBox.myCenterPos);

	const CU::Vector3f& min = myBoundingBox.myMinPos;
	const CU::Vector3f& max = myBoundingBox.myMaxPos;

	CU::Vector3f aabbCorners[8] =
	{
		CU::Vector3f(min),
		CU::Vector3f(max.x, min.y, min.z),
		CU::Vector3f(min.x, max.y, min.z),
		CU::Vector3f(min.x, min.y, max.x),
		CU::Vector3f(max),
		CU::Vector3f(min.x, max.y, max.z),
		CU::Vector3f(max.x, min.y, max.z),
		CU::Vector3f(max.x, max.y, min.z)
	};

	float dot = 0.0f;

	float xMax = FLT_MIN;
	float yMax = FLT_MIN;
	float zMax = FLT_MIN;

	float xMin = FLT_MAX;
	float yMin = FLT_MAX;
	float zMin = FLT_MAX;

	for (int i = 0; i < 8; ++i)
	{
		// X
		dot = lightSpace.myRightVector.Dot(aabbCorners[i]);
		if (dot < xMin)
			xMin = dot;
		else if (dot > xMax)
			xMax = dot;

		// Y
		dot = lightSpace.myUpVector.Dot(aabbCorners[i]);
		if (dot < yMin)
			yMin = dot;
		else if (dot > yMax)
			yMax = dot;

		// Z
		dot = lightSpace.myForwardVector.Dot(aabbCorners[i]);
		if (dot < zMin)
			zMin = dot;
		else if (dot > zMax)
			zMax = dot;

	}

	float width = xMax - xMin;
	float height = yMax - yMin;
	float depth = zMax - zMin;
	lightSpace.Move({ 0.0f, 0.0f, zMin});

	myRenderCamera.GetCamera().ReInit(width, height, 1.0f, depth);
	myRenderCamera.GetCamera().SetTransformation(lightSpace);

	myData.lightProjection = myRenderCamera.GetCamera().GetProjection();
	myData.lightSpace = lightSpace.GetInverted();
}
