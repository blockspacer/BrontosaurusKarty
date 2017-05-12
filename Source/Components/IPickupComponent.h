#pragma once
#include "Component.h"
class IPickupComponent : public CComponent
{
public:
	IPickupComponent();
	~IPickupComponent();

	virtual void ReInit();

	void SetActive(const bool aFlag);
	void SetNetworkId(const int aID);

	void SetRespawnTime(const float aRespawnTime);
	const float GetRespawnTime() const;

	const bool GetIsActive() const;
	const int GetNetworkId() const;

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	virtual const bool GetShouldReset() const;

	virtual void DoMyEffect(CComponent* theCollider) = 0;

	void Update(const float aDeltaTime);
protected:
	float myRespawnTime;
	float myRespawnTimer;
	int myNetworkId;
private:
	bool myHasBeenPickedUp;
};

