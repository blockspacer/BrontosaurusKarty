#include "stdafx.h"
#include "IPickupComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../BrontosaurusEngine/Engine.h"

#define VERTICAL_OFFSET 0.25f
#define VERTICAL_RATE 1.0f

IPickupComponent::IPickupComponent()
{
	myHasBeenPickedUp = false;
	myRespawnTime = 0.0f;
	myRespawnTimer = 0.0f;
}


IPickupComponent::~IPickupComponent()
{
}

void IPickupComponent::ReInit()
{
	SetActive(true);
}

void IPickupComponent::SetActive(const bool aFlag)
{
	myHasBeenPickedUp = !aFlag;
	if (GetParent() != nullptr)
	{
		SComponentMessageData data;
		data.myBool = aFlag;
		GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, data);
		myRespawnTimer = 0.0f;
	}
}

void IPickupComponent::SetNetworkId(const int aID)
{
	myNetworkId = aID;
}

//parameter as Seconds.Milliseconds
void IPickupComponent::SetRespawnTime(const float aRespawnTime)
{
	myRespawnTime = aRespawnTime;
}

const float IPickupComponent::GetRespawnTime() const
{
	return myRespawnTime;
}

const int IPickupComponent::GetNetworkId() const
{
	return myNetworkId;
}

void IPickupComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if(aMessageData.myComponentTypeAdded == eComponentType::eModel)
		{
			SComponentMessageData message;
			message.myComponentTypeAdded = myType;
			GetParent()->NotifyComponents(eComponentMessageType::eAddComponent, message);
		}
		if (aMessageData.myComponent == this)
		{
			GetParent()->GetLocalTransform().myPosition.y += VERTICAL_OFFSET;
			GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}
		break;
	case eComponentMessageType::eOnTriggerEnter:
		if (myHasBeenPickedUp == false)
		{

			IPickupComponent::DoMyEffect(aMessageData.myComponent);
			DoMyEffect(aMessageData.myComponent);
		}
		break;
	default:
		break;
	}
}

const bool IPickupComponent::GetShouldReset() const
{
	return true;
}

const bool IPickupComponent::GetIsActive() const
{
	return myHasBeenPickedUp;
}

void IPickupComponent::DoMyEffect(CComponent* theCollider)
{
	SetActive(false);
}

void IPickupComponent::Update(const float aDeltaTime)
{
	if (myRespawnTime != 0.0f)
	{
		if (GetIsActive() == true)
		{
			myRespawnTimer += aDeltaTime;
			if (myRespawnTimer >= myRespawnTime)
			{
				SetActive(true);
			}
			return;
		}
	}
	GetParent()->GetLocalTransform().RotateAroundAxis(aDeltaTime * 3.141592 * 0.5f, CU::Axees::Y);
	float offset = sin(ENGINE->GetTime().GetSeconds() * 3.141592 * VERTICAL_RATE);

	offset *= VERTICAL_OFFSET;
	offset *= aDeltaTime;

	GetParent()->GetLocalTransform().myPosition.y += offset;
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}
