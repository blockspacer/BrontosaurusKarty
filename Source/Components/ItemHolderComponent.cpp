#include "stdafx.h"
#include "ItemHolderComponent.h"
#include "ItemFactory.h"


CItemHolderComponent::CItemHolderComponent(CItemFactory& aItemFactory): myItemFactory(aItemFactory)
{
	myItem = eItemTypes::eNone;
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
		if (myItem == eItemTypes::eNone)
		{
			myItem = static_cast<eItemTypes>(aMessageData.myInt);
		}
		break;
	case eComponentMessageType::eUseItem:
 		myItemFactory.CreateItem(myItem,this);
		myItem = eItemTypes::eNone;
		break;
	default:
		break;
	}
}
