#pragma once
#include "Component.h"
class CRespawnerComponent : public CComponent
{
public:
	CRespawnerComponent();
	~CRespawnerComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Update();
private:
	void UpdateGroundedPosition();
private:
	CU::Vector3f myLastGroundedPosition;

};

