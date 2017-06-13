#include "stdafx.h"
#include "C3DSpriteComponent.h"
#include "Engine.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "RenderMessages.h"
#include "Renderer.h"
#include "Scene.h"


C3DSpriteComponent::C3DSpriteComponent(CScene& aScene, const std::string& aPath, const CU::Vector2f& aSize, const CU::Vector2f& aPivot, const CU::Vector4f& aRect, const CU::Vector4f& aColor) :
	myPivot(aPivot),
	myRect(aRect),
	myColor(aColor),
	myScene(aScene)
{
	mySprite = SPRMGR.CreateSprite(aPath.c_str());
	CU::Vector2f windowSize(WINDOW_SIZE);
	CU::Vector2f textureSize(mySprite->GetTextureSizeFloat());
	CU::Vector2f normalizedSize = textureSize / windowSize;
	mySize = normalizedSize * aSize;
	myScene.AddSprite(this);
}

C3DSpriteComponent::~C3DSpriteComponent()
{
	myScene.RemoveSprite(this);
	SPRMGR.DestroySprite(mySprite);
	mySprite = nullptr;
}

void C3DSpriteComponent::Destroy()
{
	delete this;
}

void C3DSpriteComponent::Render()
{
	SRender3DSpriteMessage* renderMessage = new SRender3DSpriteMessage();
	renderMessage->myPosition = GetParent()->GetWorldPosition();
	renderMessage->myPosition.w = 1.f;
	renderMessage->mySize = mySize;
	renderMessage->myRect = myRect;
	renderMessage->myPivot = myPivot;
	renderMessage->myColor = myColor;
	renderMessage->mySprite = mySprite;
	renderMessage->myRotation = 0.f;

	RENDERER.AddRenderMessage(renderMessage);
}
