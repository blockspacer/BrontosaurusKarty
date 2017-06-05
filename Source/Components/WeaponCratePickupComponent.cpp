#include "stdafx.h"
#include "WeaponCratePickupComponent.h"
#include "ItemFactory.h"
#include "../Audio/AudioInterface.h"


CItemPickupComponent::CItemPickupComponent(CItemFactory& aItemFactory) : myItemFactory(aItemFactory)
{
	myScale = 1.0f;
	myTimer = 0.0f;
}


CItemPickupComponent::~CItemPickupComponent()
{
}

void CItemPickupComponent::Update(const float aDeltaTime)
{
	IPickupComponent::Update(aDeltaTime);
	if (GetIsActive() == false)
	{
		myTimer += aDeltaTime;
		if (myTimer < 0.05f)
		{
			SComponentMessageData data;
			data.myBool = false;
			GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
		}
		else
		{
			SComponentMessageData data;
			data.myBool = true;
			GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
		}
		if (myScale < 1.0f)
		{
			myScale += (aDeltaTime * 0.5f);
			GetParent()->GetLocalTransform().SetScale(CU::Vector3f(myScale, myScale, myScale));
		}
		else if (myScale > 1.0f)
		{
			myScale = 1.0f;
			GetParent()->GetLocalTransform().SetScale(CU::Vector3f(myScale, myScale, myScale));
		}
	}
}

void CItemPickupComponent::DoMyEffect(CComponent* theCollider)
{
	IPickupComponent::DoMyEffect(theCollider);
	SComponentMessageData data;
	data.myInt = static_cast<int>(myItemFactory.RandomizeItem(theCollider));
	theCollider->GetParent()->NotifyComponents(eComponentMessageType::eGiveItem, data);
	SComponentMessageData sound; sound.myString = "PlayPickup";
	theCollider->GetParent()->NotifyOnlyComponents(eComponentMessageType::ePlaySound, sound);
	myScale = 0.001f;
	myTimer = 0.0f;

	GetParent()->GetLocalTransform().SetScale(CU::Vector3f(myScale, myScale, myScale));
}