#include "stdafx.h"
#include "ItemHolderComponent.h"
#include "ItemFactory.h"


CItemHolderComponent::CItemHolderComponent(CItemFactory& aItemFactory): myItemFactory(aItemFactory)
{
}


CItemHolderComponent::~CItemHolderComponent()
{
}

void CItemHolderComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		break;
	case eComponentMessageType::eGiveItem:
		myItem = static_cast<eItemTypes>(aMessageData.myInt);
		break;
	case eComponentMessageType::eUseItem:
		myItemFactory.CreateItem(myItem,this);
		break;
	default:
		break;
	}
}
