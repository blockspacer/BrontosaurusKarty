#include "stdafx.h"
#include "TerrainComponent.h"

Component::CTerrainComponent::CTerrainComponent(const float aSpeedModifier) : mySpeedModifier(aSpeedModifier)
{
}

Component::CTerrainComponent::~CTerrainComponent()
{
}

void Component::CTerrainComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CComponent::Receive(aMessageType, aMessageData);
}

bool Component::CTerrainComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eGetTerrainModifier:
		{
			aQuestionData.myFloat = mySpeedModifier;
			return true;
		}
	}
	return CComponent::Answer(aQuestionType, aQuestionData);
}

void Component::CTerrainComponent::Destroy()
{
	delete this;
}
