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
	case eComponentMessageType::eReleaseItem:

		if (aMessageData.myVector2f != CU::Vector2f::Zero)
		{
			myItemFactory.ReleaseItem(this, aMessageData.myVector2f);
			break;
		}

		myItemFactory.ReleaseItem(this);


		break;
	default:
		break;
	}
}

bool CItemHolderComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eGetHoldItemType:
	{
		if(myItem != eItemTypes::eNone)
		{
			aQuestionData.myInt = static_cast<int>(myItem);
			return true;
		}
		break;
	}
	default:
		break;
	}

	return false;
}