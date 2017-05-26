#include "stdafx.h"
#include "HazardComponent.h"
#include "../Physics/CollisionLayers.h"
#include "ColliderComponent.h"


CHazardComponent::CHazardComponent()
{
	myIsActive = false;
	myIsPermanent = false;
}


CHazardComponent::~CHazardComponent()
{
}

void CHazardComponent::SetToPermanent()
{
	myIsPermanent = true;
}

void CHazardComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTurnOffHazard:
	{
		myIsActive = false;
		break;
	}
	case eComponentMessageType::eActivate:
	{
		myIsActive = true;
		break;
	}
	case eComponentMessageType::eMakeInvurnable:
	{
		myIsActive = true;
		break;
	}
	case eComponentMessageType::eOnTriggerEnter:
	{
		if(myIsActive == true)
		{
			CColliderComponent* collider = reinterpret_cast<CColliderComponent*>(aMessageData.myComponent);
			const SColliderData* data = collider->GetData();

			if (data->myLayer == Physics::eKart)
			{
				aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eGotHit, SComponentMessageData());
				if (myIsPermanent == false)
				{
					GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData()); //can be a problem with unity parenting...
				}
			}
		}
	}
	break;
	default:
		break;
	}
}
