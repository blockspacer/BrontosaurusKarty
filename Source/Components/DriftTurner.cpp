#include "stdafx.h"
#include "DriftTurner.h"
#include "../Physics/PhysXHelper.h"


Component::CDriftTurner::CDriftTurner()
{
}


Component::CDriftTurner::~CDriftTurner()
{
}

void Component::CDriftTurner::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CComponent::Receive(aMessageType, aMessageData);
	switch (aMessageType)
	{
	case eComponentMessageType::eDoDriftBobbing:
	{
		GetParent()->GetLocalTransform().SetRotation(CU::Matrix33f::CreateRotateAroundY(aMessageData.myFloat));
		//myTargetTurn = CU::Matrix33f::CreateRotateAroundY(aMessageData.myFloat);
	}
		break;
	case eComponentMessageType::eCancelDriftBobbing:
		GetParent()->GetLocalTransform().SetRotation(CU::Matrix33f::Identity);
		myTargetTurn = CU::Matrix33f::Identity;
		break;
	case eComponentMessageType::eUpdate:
		//DoUpdate(aMessageData.myFloat);
		break;
	}
}

bool Component::CDriftTurner::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	return false;
}

void Component::CDriftTurner::Destroy()
{
	delete this;
}

const float sleprVal = 10.f;
void Component::CDriftTurner::DoUpdate(const float aFloat)
{
	GetParent()->GetLocalTransform().SetRotation(Physics::Slerp(GetParent()->GetLocalTransform().GetRotation(), myTargetTurn, ( 1.f / sleprVal) * aFloat).GetRotation());
}
