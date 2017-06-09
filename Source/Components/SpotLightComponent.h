#pragma once
#include "Component.h"

class CScene;
typedef unsigned int InstanceID;

class CSpotLightComponent :	public CComponent
{
public:
	CSpotLightComponent(CScene& aScene);
	~CSpotLightComponent();
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void SetColor(const CU::Vector3f& aColor);
	void SetIntensity(const float aIntensity);
	void SetRange(const float aRange);
	void SetSpotAngle(const float aSpotAngle);
private:
	CScene& myScene;
	InstanceID myLightID;

};
