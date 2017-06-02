#include "stdafx.h"
#include "BlueShellBehaviourComponent.h"
#include "LapTrackerComponentManager.h"


CBlueShellBehaviourComponent::CBlueShellBehaviourComponent(CU::GrowingArray<CGameObject*>& aListOfKartObjects)
{
	myKartObjects = &aListOfKartObjects;

	myIsActive = false;
	myTeleportDelay = 3.5f;
	myElapsedTime = 0;
	mySpeed = 90;
	myVelocity = CU::Vector3f::UnitZ * mySpeed;
}


CBlueShellBehaviourComponent::~CBlueShellBehaviourComponent()
{
}

void CBlueShellBehaviourComponent::Update(const float aDeltaTime)
{
	CU::Matrix44f target;

	if (myIsActive == false)
	{
		return;
	}

	if (myElapsedTime < myTeleportDelay)
	{
		myElapsedTime += aDeltaTime;
		CU::Vector3f flyUp(0, 20, 0);
		GetParent()->Move(flyUp*aDeltaTime);

		if (myElapsedTime >= myTeleportDelay)
		{
			for (int i = 0; i < myKartObjects->Size(); i++)
			{
				char placement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myKartObjects->At(i));
				if (placement == 1)
				{
					target = myKartObjects->At(i)->GetToWorldTransform();
					break;
				}
			}

			target.Move(flyUp);
			GetParent()->GetToWorldTransform().SetPosition(target.GetPosition());
		}

		return;
	}




	for (int i = 0; i < myKartObjects->Size(); i++)
	{
		char placement = CLapTrackerComponentManager::GetInstance()->GetSpecificRacerPlacement(myKartObjects->At(i));
		if (placement == 1)
		{
			target = myKartObjects->At(i)->GetToWorldTransform();
			break;
		}
	}

	CU::Matrix44f transform = GetParent()->GetToWorldTransform();

	transform.LookAt(target.GetPosition());

	transform.Move(myVelocity*aDeltaTime);

	GetParent()->SetWorldTransformation(transform);

	SComponentMessageData data; data.myFloat = aDeltaTime;
	GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, data);

}

void CBlueShellBehaviourComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case (eComponentMessageType::eDeactivate):
		myIsActive = false;
		break;
	case(eComponentMessageType::eReInitBlueShell):
	{
		myIsActive = true;
		myElapsedTime = 0;
		break;
	}

	}
}