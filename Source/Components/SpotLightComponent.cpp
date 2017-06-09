#include "stdafx.h"
#include "SpotLightComponent.h"
#include "SpotLightInstance.h"
#include "Scene.h"

CSpotLightComponent::CSpotLightComponent(CScene& aScene)
	: myScene(aScene)
{
	myLightID = aScene.AddSpotLightInstance(CSpotLightInstance());
	myType = eComponentType::eSpotLight;
}

CSpotLightComponent::~CSpotLightComponent()
{
}

void CSpotLightComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	CSpotLightInstance* spotLight = myScene.GetSpotLightInstance(myLightID);
	if (!spotLight) return;

	switch (aMessageType)
	{
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
		spotLight->SetPosition(GetParent()->GetToWorldTransform().GetPosition());
		spotLight->SetRotation(GetParent()->GetToWorldTransform().GetRotation());
		break;
	}
}

void CSpotLightComponent::SetColor(const CU::Vector3f& aColor)
{
	CSpotLightInstance* spotLight = myScene.GetSpotLightInstance(myLightID);
	if (!spotLight) return;
	spotLight->SetColor(aColor);
}

void CSpotLightComponent::SetIntensity(const float aIntensity)
{
	CSpotLightInstance* spotLight = myScene.GetSpotLightInstance(myLightID);
	if (!spotLight) return;
	spotLight->SetInstensity(aIntensity);
}

void CSpotLightComponent::SetRange(const float aRange)
{
	CSpotLightInstance* spotLight = myScene.GetSpotLightInstance(myLightID);
	if (!spotLight) return;
	spotLight->SetRange(aRange);
}

void CSpotLightComponent::SetSpotAngle(const float aSpotAngle)
{
	CSpotLightInstance* spotLight = myScene.GetSpotLightInstance(myLightID);
	if (!spotLight) return;
	spotLight->SetSpotAngle(aSpotAngle);
}
