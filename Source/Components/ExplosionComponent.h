#pragma once
#include "Component.h"
class CExplosionComponent: public CComponent
{
public:
	CExplosionComponent();
	~CExplosionComponent();


	void Update(const float aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:

	CU::Vector3f myPosition;

	float myDuration;
	float myElapsedTime;

	float myScale;

	bool myIsActive;

};

