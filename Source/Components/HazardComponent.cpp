#include "stdafx.h"
#include "HazardComponent.h"
#include "../Physics/CollisionLayers.h"
#include "ColliderComponent.h"


CHazardComponent::CHazardComponent()
{
}


CHazardComponent::~CHazardComponent()
{
}

void CHazardComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
	{
		CColliderComponent* collider = reinterpret_cast<CColliderComponent*>(aMessageData.myComponent);
		const SColliderData* data = collider->GetData();

		if (data->myLayer == Physics::eKart)
		{
			aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eGotHit, SComponentMessageData());
			GetParent()->NotifyComponents(eComponentMessageType::eDeactivate, SComponentMessageData());
		}
	}
	break;
	default:
		break;
	}
}
