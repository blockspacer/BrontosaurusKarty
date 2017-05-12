#pragma once
#include "ItemTypes.h"

class ItemFactory
{
public:
	ItemFactory();
	~ItemFactory();

	eItemTypes RandomizeItem();

	int CreateItem(const eItemTypes aItemType);

};

