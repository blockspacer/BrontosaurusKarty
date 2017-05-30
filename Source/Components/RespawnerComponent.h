#pragma once
#include "Component.h"

struct SLastGroundData
{
	CU::Vector3f lastGroundPosition;
	float timeSinceCreated = 0;
};

class CRespawnerComponent : public CComponent
{
public:
	CRespawnerComponent();
	~CRespawnerComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Update(float aDeltaTime);
private:
	void UpdateGroundedPosition(float aDeltaTime);
private:
	CU::GrowingArray<SLastGroundData> myGroundData;
	float myTakeNewGroundCountDown;
	float myCurrentSplinePositionY;
};

