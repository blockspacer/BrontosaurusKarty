#pragma once
#include "IPickupComponent.h"
class CWeaponCratePickupComponent :
	public IPickupComponent
{
public:
	CWeaponCratePickupComponent();
	~CWeaponCratePickupComponent();
	void DoMyEffect() override;
private:
};

