#pragma once
#include "ItemTypes.h"

class CItemFactory
{
public:
	CItemFactory();
	~CItemFactory();

	eItemTypes RandomizeItem();

	int CreateItem(const eItemTypes aItemType, CComponent* userComponent);

};

