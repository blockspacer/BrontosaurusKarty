#pragma once

#include "../CommonUtilities/Camera.h"

#include "RenderPackage.h"
#include "FullScreenHelper.h"
#include "BufferStructs.h"
#include "Lights.h"
#include "ParticleEmitter.h"
#include "ShadowBuffers.h"
#include "Colour.h"
#include "RenderCamera.h"
#include "GUIElement.h"
#include "../Components/C3DSpriteComponent.h"

struct ID3D11RenderTargetView;

class CSkybox;
class CModelInstance;
class CModel;
class CSprite;
class CText;
class CText;
class CPointLightInstance;
class CParticleEmitter;
class CStreakEmitterInstance;

struct SPixelConstantBuffer;
struct SParticle;

enum class eRasterizerState : int;
enum class eBlendState : int;
enum class eDepthStencilState : int;
enum class eSamplerState : int;
enum class eAlignment : int;

enum ELUTType : char;

typedef unsigned int ParticleEmitterID;

namespace CU
{
	class Camera;
}

namespace Lights
{
	struct SDirectionalLight;
}


struct SRenderMessage
{
	enum class eRenderMessageType
	{
		eSetCamera,
		eRenderCameraQueue,
		eRenderModel,
		eRenderModelDepth,
		eRenderModelDeferred,
		eRenderModelShadow,
		eRenderModelInstanced,
		eRenderModelBatches,
		eRenderGUIModel,
		eRenderSprite,
		eChangeStates,
		eRenderSkybox,
		eRenderParticles,
		eRenderStreak,
		eRenderFire,
		eRenderText,
		eClearRenderPackage,
		eActivateRenderTo,
		eActivateRenderPackage,
		eRenderFullscreenEffect,
		eRenderDebugObjs,
		eRenderToInterediate,
		eSetShadowBuffer,
		eRenderLineBuffer,
		eRenderNavMesh,
		eSetRTV,
		eSetCubemapResource,
		eClear,
		eRenderCallback,
		eRenderDirectionalLight,
		eRenderPointLight,
		eRenderSpotLight,
		eRenderBar,
		eCreateGuiElement,
		eRenderToGui,
		eLUTFADECOLORGRADE,
		eClearGui,
		eRenderDecal,
		e3DSprite,
	};

	SRenderMessage(const eRenderMessageType aRenderMessageType);
	virtual ~SRenderMessage();

	eRenderMessageType myType;

};

struct SRenderDirectionalLight : SRenderMessage
{
	SRenderDirectionalLight();
	Lights::SDirectionalLight directionalLight;
};

struct SRenderPointLight : SRenderMessage
{
	SRenderPointLight();
	Lights::SPointLight pointLight;
};

struct SRenderSpotLight : SRenderMessage
{
	SRenderSpotLight();
	Lights::SSpotLight spotLight;
	CU::Matrix33f rotation;
};

struct SRenderDecal : SRenderMessage
{
	SRenderDecal();
	CU::Matrix44f myWorldSpace;
	SDecalData myData;
};

struct SSetCubemapResource : SRenderMessage
{
	SSetCubemapResource();
	//Incr refcount
	ID3D11ShaderResourceView* mySRV;
};


struct SClear : SRenderMessage
{
	SClear();
	//Incr refcount
	ID3D11RenderTargetView* myRTV;
	ID3D11DepthStencilView* myDSV;
};


struct SSetRTVMessage : SRenderMessage
{
	SSetRTVMessage();
	//Incr refcount
	D3D11_VIEWPORT* myViewport;
	ID3D11RenderTargetView* myRTV;
};

struct SActivateRenderToMessage : SRenderMessage
{
	SActivateRenderToMessage();
};


struct SClearRenderPackageMessage : SRenderMessage
{
	SClearRenderPackageMessage();
	CRenderPackage myRenderPackage;
};


struct SRenderNavmeshMessage : SRenderMessage
{
	SRenderNavmeshMessage();
	CU::Matrix44f myTransformation;
	CModel* myModel;
};


struct SRenderFullscreenEffectMessage : SRenderMessage
{
	SRenderFullscreenEffectMessage();

	CU::Vector4f myRect;

	bool myFirstUseDepthResource;
	bool mySecondUseDepthResource;

	// If effect with only one renderpackage set only first
	CRenderPackage myFirstPackage;
	CRenderPackage mySecondPackage;

	CFullScreenHelper::eEffectType myEffectType;

};

struct SLutFadeColorGrade : SRenderMessage
{
	SLutFadeColorGrade();
	float myFadeTime;
	ELUTType myFadeTo, myFadeFrom;
};

struct SActivateRenderPackageMessage : SRenderMessage
{
	SActivateRenderPackageMessage();
	CRenderPackage myRenderPackage;
	CRenderPackage mySecondRenderPackage;
	bool unbindShadowBuffer;
	bool useSecondPackage;
};

struct SRenderToIntermediate : SRenderMessage
{
	SRenderToIntermediate();
	CRenderPackage myRenderPackage;
	CU::Vector4f myRect = CU::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
	bool useDepthResource;
};

struct SRenderCameraQueueMessage : SRenderMessage
{
	~SRenderCameraQueueMessage();
	SRenderCameraQueueMessage();
	CRenderCamera myRenderCamera;
	bool RenderDepth;
	int myOwnerIndex;
};

struct SRenderModelMessage : SRenderMessage
{
	SRenderModelMessage();
	SForwardRenderModelParams myRenderParams;
	int myModelID;
};

struct SRenderModelDeferredMessage : SRenderMessage
{
	SRenderModelDeferredMessage();
	~SRenderModelDeferredMessage();
	SDeferredRenderModelParams myRenderParams;
	int myModelID;
	CTexture* myVertexTexture;
};

struct SRenderModelInstancedMessage : SRenderMessage
{
	SRenderModelInstancedMessage();
	SDeferredRenderModelParams myRenderParams;
	int myModelID;
};

struct SRenderModelBatches : SRenderMessage
{
	SRenderModelBatches();
	bool myRenderToDepth = false;
	ID3D11PixelShader* myPixelShader = nullptr;
};

struct SRenderModelShadowMessage : SRenderMessage
{
	SRenderModelShadowMessage();
	SDeferredRenderModelParams myRenderParams;
	int myModelID;
};

struct SRenderModelDepthMessage : SRenderModelMessage
{
	SRenderModelDepthMessage();
};

struct SSetShadowBuffer : SRenderMessage
{
	SSetShadowBuffer();
	SCascadeBuffer cascadeBuffer;
	SBakedShadowBuffer myShadowBufferData;
	CRenderPackage myShadowBuffer;
};

struct SRenderParticlesMessage : SRenderMessage
{
	SRenderParticlesMessage();
	~SRenderParticlesMessage();
	ParticleEmitterID particleEmitter;
	CU::GrowingArray<SParticle, unsigned int, false> particleList;
	CU::Matrix44f toWorld;
	CParticleEmitter::RenderMode renderMode = CParticleEmitter::RenderMode::eMetaBall;
};

struct SRenderStreakMessage : SRenderMessage
{
	SRenderStreakMessage();
	CStreakEmitterInstance* streakEmitter;
};

struct SRenderFireMessage : SRenderMessage
{
	SRenderFireMessage();
	CU::Matrix44f myToWorldMatrix;
	int myFireID;
};

struct SRenderSpriteMessage : SRenderMessage
{
	SRenderSpriteMessage();
	CU::Vector2f myPosition;
	CU::Vector2f mySize;
	CU::Vector2f myPivot;
	CU::Vector4f myRect;
	CU::Vector4f myColor;
	CSprite* mySprite;
	float myRotation;
};

struct SRender3DSpriteMessage : SRenderMessage
{
	SRender3DSpriteMessage();
	CU::Vector4f myPosition;
	CU::Vector2f mySize;
	CU::Vector2f myPivot;
	CU::Vector4f myRect;
	CU::Vector4f myColor;
	CSprite* mySprite;
	float myRotation;
	int myOwnerIndex;
};

struct SRenderSkyboxMessage : SRenderMessage
{
	SRenderSkyboxMessage();
	SRenderSkyboxMessage(const SRenderSkyboxMessage& aCopy) = delete;
	~SRenderSkyboxMessage();
	CSkybox* mySkybox;
};

struct SSetCameraMessage : SRenderMessage
{
	SSetCameraMessage();
	CU::Camera myCamera;
};


struct SChangeStatesMessage : SRenderMessage
{
	SChangeStatesMessage();
	eRasterizerState myRasterizerState;
	eBlendState myBlendState;
	eDepthStencilState myDepthStencilState;
	eSamplerState mySamplerState;
};


struct SRenderTextMessage : SRenderMessage
{
	SRenderTextMessage();
	CU::Vector4f myColor;
	CU::GrowingArray<std::wstring> myStrings;
	CU::Vector2f myPosition;
	CText *myText;
	CU::Vector2f myLineHeight;
	eAlignment myAlignement;
};


struct SRenderLineBuffer : SRenderMessage
{
	SRenderLineBuffer(const CU::GrowingArray<char, unsigned int, false>& aLineBuffer);
	CU::GrowingArray<char, unsigned int, false> myLineBuffer;
};

struct SRenderBarMessage : SRenderMessage
{
	SRenderBarMessage(): SRenderMessage(eRenderMessageType::eRenderBar), myCurrentLevel(0) {}

	CU::Vector4f myRect;
	CU::Colour myFullColour;
	CU::Colour myEmptyColour;
	CU::Colour myBackgroundColour;
	float myCurrentLevel;
};

struct SCreateOrClearGuiElement : SRenderMessage
{
	SCreateOrClearGuiElement(const std::wstring& anElementName, const SGUIElement& aGUIElement, const CU::Vector2ui aPixelSize);

	const std::wstring myElementName;
	const SGUIElement myGuiElement;
	const CU::Vector2ui myPixelSize;
};

struct SRenderToGUI : SRenderMessage
{
	SRenderToGUI(const std::wstring& anElementName, SRenderMessage * const aRenderMessage);
	~SRenderToGUI();

	const std::wstring myElementName;
	SRenderMessage* myRenderMessage;
};

struct SClearGui: SRenderMessage
{
	SClearGui() : SRenderMessage(eRenderMessageType::eClearGui){}
};

struct SRenderCallback : SRenderMessage
{
	SRenderCallback() : SRenderMessage(SRenderMessage::eRenderMessageType::eRenderCallback) {};
	std::function<void()> myFunction;
};

struct S3DSprite : SRenderMessage
{
	S3DSprite() : SRenderMessage(SRenderMessage::eRenderMessageType::e3DSprite){}
	CU::Matrix44f myTransformation;
	CU::Vector4f myLocalPosition;
	CU::Vector2f mySize;
};
