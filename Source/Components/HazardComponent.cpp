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
	case eComponentMessageType::eTurnOnHazard:
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
			if (collider->GetParent() == GetParent())
			{
				return;
			}
			if (data->myLayer == Physics::eKart || data->myLayer == Physics::eHazzard)
			{
				SComponentMessageData collidedData;
				collidedData.myComponent = collider;
				GetParent()->NotifyOnlyComponents(eComponentMessageType::eHazzardCollide, collidedData);

				aMessageData.myComponent->GetParent()->NotifyOnlyComponents(eComponentMessageType::eGotHit, SComponentMessageData());
				if (myIsPermanent == false)
				{
					GetParent()->NotifyOnlyComponents(eComponentMessageType::eDeactivate, SComponentMessageData()); //can be a problem with unity parenting...
					GetParent()->NotifyOnlyComponents(eComponentMessageType::eDeactivateEmitter, SComponentMessageData());
					GetParent()->NotifyOnlyComponents(eComponentMessageType::eTurnOffThePointLight, SComponentMessageData());
				}
			}
		}
	}
	break;
	default:
		break;
	}
}
