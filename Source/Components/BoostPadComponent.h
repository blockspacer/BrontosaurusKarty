#pragma once
#include "Component.h"
class CBoostPadComponent : public CComponent
{
public:
	CBoostPadComponent();
	~CBoostPadComponent();

	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	void Update();
private:
	CU::GrowingArray<CGameObject*> myCollidesWithObjects;
};

