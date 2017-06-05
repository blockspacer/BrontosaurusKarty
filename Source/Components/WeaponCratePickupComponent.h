#pragma once
#include "IPickupComponent.h"

class CItemFactory;

class CItemPickupComponent :
	public IPickupComponent
{
public:
	CItemPickupComponent(CItemFactory& aItemFactory);
	~CItemPickupComponent();
	void DoMyEffect(CComponent* theCollider) override;
	void Update(const float aDeltaTime) override;
private:
	CItemFactory& myItemFactory;
	float myScale;
	float myTimer;
	bool myFlippedVisibility;
};

