#pragma once
#include "Component.h"

class CStreakEmitterInstance;

class CStreakEmitterComponent : public CComponent
{
public:
	CStreakEmitterComponent();
	~CStreakEmitterComponent();
	void Init(CStreakEmitterInstance* aParticleEmitterInstance);
	void Update(CU::Time aDeltaTime);

	inline CStreakEmitterInstance* GetEmitter();

	void Activate();
	void Deactivate();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData);
	void Destroy() override;

private:
	CStreakEmitterInstance* myStreakEmitterInstance;
};

CStreakEmitterInstance* CStreakEmitterComponent::GetEmitter()
{
	return myStreakEmitterInstance;
}
