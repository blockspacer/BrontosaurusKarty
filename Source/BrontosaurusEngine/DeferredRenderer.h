#pragma once
#include "GeometryBuffer.h"
#include "RenderMessages.h"

#ifndef _RETAIL_BUILD
#define _ENABLE_RENDERMODES
#endif

struct SRenderMessage;
struct ID3D11Buffer;
class CDXFramework;
class CLightModel;
class CFullScreenHelper;
class CTexture;

#ifdef _ENABLE_RENDERMODES
namespace CU
{
	class InputWrapper;
}
#endif

class CDeferredRenderer
{
	friend CRenderer;

	struct SCameraStruct
	{
		CU::Matrix44f prjInv;
		CU::Matrix44f camTW;
	};
public:
	CDeferredRenderer();
	~CDeferredRenderer();

	
	void DoRenderQueue(CRenderer & aRenderer);

	void AddRenderMessage(SRenderMessage* aRenderMessage);
	void UpdateCameraBuffer(const CU::Matrix44f & aCameraSpace, const CU::Matrix44f & aProjectionInverse);
	void SetGeometryBuffer(CGeometryBuffer& myGbuffer);

	void DoDecals(CRenderer& aRenderer);

	void DoLightingPass(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer, const CU::Matrix44f& aCameraTransform, const CU::Matrix44f& aProjection);
	ID3D11DepthStencilView* GetDepthStencil();
	ID3D11ShaderResourceView* GetDepthResource();
	CRenderPackage& GetFirstPackage();
	CRenderPackage& GetSecondPackage();
	void Do3DSprites(const CU::Matrix44f& aMatrix44, const CU::Matrix44f& aProjection, const CU::Vector2f& aViewportSize, int aPlayerIndex);

private:
	
	void DoAmbientLighting(CFullScreenHelper& aFullscreenHelper);
	void DoDirectLighting(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer);

	void RenderDirectionalLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper);
	void RenderPointLight(SRenderMessage* aRenderMessage);
	void RenderSpotLight(SRenderMessage* aRenderMessage);

	void ActivateIntermediate();
	void SetRMAOSRV();

	void SetCBuffer();


	void InitLightModels();
	void InitDecalModel();

	void DoSSAO(CFullScreenHelper& aFullscreenHelper);

private:
	CU::GrowingArray<SRenderMessage*> myRenderMessages;
	CU::GrowingArray<SRenderMessage*> myLightMessages;
	CU::GrowingArray<int> myBatchedModelIds;
	CU::GrowingArray<SRenderMessage*> myDecalMessages;
	CU::GrowingArray<SRender3DSpriteMessage*> my3DSprites;

	CLightModel* myPointLightModel;
	CLightModel* mySpotLightModel;
	CLightModel* myDecalModel;

	ID3D11Buffer* myProjectionInverseBuffer;
	ID3D11Buffer* myDirectionalLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11Buffer* mySpotLightBuffer;
	ID3D11Buffer* myDecalBuffer;

	CGeometryBuffer* myGbuffer;
	
	CRenderPackage myIntermediatePackage;
	
	CDXFramework* myFramework;
	CTexture* mySSAORandomTexture;
	CTexture* myDecalDiffuse;

	CRenderPackage mySSAOPackage;

#ifdef _ENABLE_RENDERMODES
	enum class ERenderMode
	{
		eDiffuse,
		eNormal,
		eRoughness,
		eMetalness,
		eAO,
		eRMAO,
		eSSAO,
		eIntermediate,
		eDepth,
		eHighlight,
		eEmissive
	} myRenderMode;
	CU::InputWrapper* myInputWrapper;
	
	void HandleInput();
#endif 
};