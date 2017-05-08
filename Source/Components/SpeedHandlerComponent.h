#pragma once
#include "Component.h"

struct SBoostData;
struct SBoostListData
{
	const SBoostData* data;
	float elapsedBoostingTime;
};
class CSpeedHandlerComponent : public CComponent
{
public:
	CSpeedHandlerComponent();
	~CSpeedHandlerComponent();

	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	void Update(float aDeltaTime);
private:
	const SBoostData* GetFastestBoost() const;
	void SendSetBoostMessage();
private:
	CU::GrowingArray<SBoostListData> myBoostList;
};

