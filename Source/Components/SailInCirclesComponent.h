#pragma once
#include "Component.h"
class CSailInCirclesComponent :	public CComponent
{
public:
	CSailInCirclesComponent(const float aRPM, const float aRadius);
	~CSailInCirclesComponent();


	void Update(const float aPastTime);


	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	float myRPS;
	float myRadius;

	CU::Vector3f myInitialPosition;
};

