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
private:
	CItemFactory& myItemFactory;

};

