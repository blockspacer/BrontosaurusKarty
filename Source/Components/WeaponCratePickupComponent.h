#pragma once
#include "IPickupComponent.h"
class CItemPickupComponent :
	public IPickupComponent
{
public:
	CItemPickupComponent();
	~CItemPickupComponent();
	void DoMyEffect() override;
private:
};

