#include "stdafx.h"
#include "ItemFactory.h"


CItemFactory::CItemFactory()
{
}


CItemFactory::~CItemFactory()
{
}

eItemTypes CItemFactory::RandomizeItem()
{
	return eItemTypes::eMushroom;
}

int CItemFactory::CreateItem(const eItemTypes aItemType, CComponent* userComponent)
{
	switch (aItemType)
	{
	case eItemTypes::eGreenShell:
		//Create a shell that can ricochet of walls
		break;
	case eItemTypes::eRedShell:
		//Create shell that homes in on the player in front of the user
		break;
	case eItemTypes::eBlueShell:
		//Create shell that homes in on the leader
		break;
	case eItemTypes::eMushroom:

		//userComponent->GetParent()->NotifyComponents(eComponentMessageType::eGiveBoost,)
		break;
	case eItemTypes::eGoldenMushroom:
		//boost and have some form of cooldown before 
		break;
	case eItemTypes::eStar:
		//make invincible and boost
		break;
	case eItemTypes::eBulletBill:
		//activate AI controller on player and make invincible and boost
		break;
	case eItemTypes::eLightning:
		//postmaster blast all
		break;
	case eItemTypes::eBanana:
		//create banana
		break;
	default:
		break;
	}

	return 0;
}
