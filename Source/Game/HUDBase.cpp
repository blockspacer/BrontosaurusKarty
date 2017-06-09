#include "stdafx.h"
#include "HUDBase.h"
#include "RenderMessages.h"
#include "Renderer.h"
#include "Engine.h"
#include "JsonValue.h"

CHUDBase::CHUDBase()
{
	myCameraOffset = { 0.f, 0.f };
}

CHUDBase::~CHUDBase()
{
}


void CHUDBase::SetGUIToAlphaBlend(std::wstring aStr)
{
	SChangeStatesMessage* changeStatesMessage = new SChangeStatesMessage();
	changeStatesMessage->myBlendState = eBlendState::eAlphaBlend;
	changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
	changeStatesMessage->mySamplerState = eSamplerState::eClamp;

	SRenderToGUI* guiChangeState = new SRenderToGUI(aStr, changeStatesMessage);
	RENDERER.AddRenderMessage(guiChangeState);
}

void CHUDBase::SetGUIToEndBlend(std::wstring aStr)
{
	SChangeStatesMessage* changeStatesMessage = new SChangeStatesMessage();
	changeStatesMessage = new SChangeStatesMessage();
	changeStatesMessage->myBlendState = eBlendState::eEndBlend;
	changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
	changeStatesMessage->mySamplerState = eSamplerState::eClamp;

	SRenderToGUI* guiChangeState = new SRenderToGUI(aStr, changeStatesMessage);
	RENDERER.AddRenderMessage(guiChangeState);
}

SHUDElement CHUDBase::LoadHUDElement(const CU::CJsonValue& aJsonValue)
{
	SHUDElement hudElement;

	hudElement.myGUIElement.myOrigin = { 0.f,0.f }; // { 0.5f, 0.5f };
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eTop] = true;
	hudElement.myGUIElement.myAnchor[(char)eAnchors::eLeft] = true;


	LoadHUDElementValues(aJsonValue, hudElement, CU::Vector2f(), CU::Vector2f());

	return hudElement;
}

void CHUDBase::LoadHUDElementValues(const CU::CJsonValue& aJsonValue, SHUDElement& aHUDElement, CU::Vector2f aPositionOffset, CU::Vector2f aSizeOffset)
{
	aHUDElement.myGUIElement.myScreenRect = CU::Vector4f(aJsonValue.at("position").GetVector2f() + myCameraOffset + aPositionOffset);

	const CU::CJsonValue sizeObject = aJsonValue.at("size");
	aHUDElement.myPixelSize.x = sizeObject.at("pixelWidth").GetUInt();
	aHUDElement.myPixelSize.y = sizeObject.at("pixelHeight").GetUInt();

	float rectWidth = sizeObject.at("screenSpaceWidth").GetFloat() + aSizeOffset.x;
	float rectHeight = sizeObject.at("screenSpaceHeight").GetFloat() + aSizeOffset.y;

	float topLeftX = aHUDElement.myGUIElement.myScreenRect.x;
	float topLeftY = aHUDElement.myGUIElement.myScreenRect.y;

	aHUDElement.myGUIElement.myScreenRect.z = rectWidth + topLeftX;
	aHUDElement.myGUIElement.myScreenRect.w = rectHeight + topLeftY;

}