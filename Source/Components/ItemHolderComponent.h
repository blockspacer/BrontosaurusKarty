#pragma once
#include "Component.h"
#include "ItemTypes.h"

class CItemHolderComponent : public CComponent
{
public:
	CItemHolderComponent();
	~CItemHolderComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;


private:
	eItemTypes myItem;

};

