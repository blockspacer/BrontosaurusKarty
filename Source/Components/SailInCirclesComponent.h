#pragma once
#include "Component.h"
class CSailInCirclesComponent :	public CComponent
{
public:
	CSailInCirclesComponent(const float aRPM, const float aVerticalRPM, const float aRadius, const float aVerticalAmplitude);
	~CSailInCirclesComponent();


	void Update(const float aPastTime);


	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	float myRPS;
	float myRPSVert;
	float myRadius;
	float myVerticalAmplitude;


	CU::Vector3f myInitialPosition;
};

