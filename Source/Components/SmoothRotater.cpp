#include "stdafx.h"
#include "SmoothRotater.h"
#include "CommonUtilities.h"



Component::CSmoothRotater::CSmoothRotater()
{
}


Component::CSmoothRotater::~CSmoothRotater()
{
}

void Component::CSmoothRotater::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CComponent::Receive(aMessageType, aMessageData);

	switch (aMessageType)
	{
	case eComponentMessageType::eRotateTowards:
		const SRotationData& data = *reinterpret_cast<SRotationData*>(aMessageData.myVoidPointer);
		DoRotation(data);
		break;;
	}
}

bool Component::CSmoothRotater::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	
	switch(aQuestionType)
	{
	case eComponentQuestionType::eGetRotatorObject:
		aQuestionData.myGameObject = GetParent();
		break;
	default:
		return false;
	}
	return true;
}

void Component::CSmoothRotater::Destroy()
{
	delete this;
}

void Component::CSmoothRotater::DoRotation(const SRotationData& aData)
{
	GetParent()->GetLocalTransform().SetRotation(aData.target);
	GetParent()->NotifyParent(eComponentMessageType::eMoving, SComponentMessageData());
}
