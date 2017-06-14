#include "stdafx.h"
#include "WeaponCratePickupComponent.h"
#include "ItemFactory.h"
#include "../Audio/AudioInterface.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"


CItemPickupComponent::CItemPickupComponent(CItemFactory& aItemFactory) : myItemFactory(aItemFactory)
{
	myScale = 1.0f;
	myTimer = 0.0f;
	myFlippedVisibility = false;
	myParticleHandle = CParticleEmitterManager::GetInstance().GetEmitterInstance("BoxDebris");
	myIsInited = false;
}


CItemPickupComponent::~CItemPickupComponent()
{
	CParticleEmitterManager::GetInstance().Release(myParticleHandle);
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
			if (myFlippedVisibility == false)
			{
				myFlippedVisibility = true;
				SComponentMessageData data;
				data.myBool = true;
				GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
			}
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
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());

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


	SComponentMessageData data2;
	data2.myBool = false;
	GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data2);

	myScale = 0.001f;
	myTimer = 0.0f;
	myFlippedVisibility = false;
	GetParent()->GetLocalTransform().SetScale(CU::Vector3f(myScale, myScale, myScale));
	if (myIsInited == false)
	{
		CU::Matrix44f matrix = GetParent()->GetToWorldTransform();
		matrix.RotateAroundAxes(3.141592f / 2.0f, 0, 0);
		CParticleEmitterManager::GetInstance().SetTransformation(myParticleHandle, matrix);
		CParticleEmitterManager::GetInstance().SetPosition(myParticleHandle, GetParent()->GetToWorldTransform().GetPosition());
	}
	CParticleEmitterManager::GetInstance().Activate(myParticleHandle);
}