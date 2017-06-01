#include "stdafx.h"
#include "WeaponCratePickupComponent.h"
#include "ItemFactory.h"
#include "../Audio/AudioInterface.h"


CItemPickupComponent::CItemPickupComponent(CItemFactory& aItemFactory) : myItemFactory(aItemFactory)
{
}


CItemPickupComponent::~CItemPickupComponent()
{
}

void CItemPickupComponent::DoMyEffect(CComponent* theCollider)
{
	IPickupComponent::DoMyEffect(theCollider);
	//Give weapon to the player... maybe it should be networked or maybe not we'll see
	SComponentMessageData data;
	data.myInt = static_cast<int>(myItemFactory.RandomizeItem(theCollider));
	theCollider->GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
	SComponentMessageData sound; sound.myString = "PlayPickup";
	theCollider->GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
}
