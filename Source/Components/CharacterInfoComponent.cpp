#include "stdafx.h"
#include "CharacterInfoComponent.h"


CCharacterInfoComponent::CCharacterInfoComponent(SParticipant::eCharacter aCharacterType, bool aIsAI)
{
	myCharacterInfo.characterType = aCharacterType;
	myCharacterInfo.isAI = aIsAI;
}

CCharacterInfoComponent::~CCharacterInfoComponent()
{
}

bool CCharacterInfoComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData & aQuestionData)
{
	if (aQuestionType == eComponentQuestionType::eGetCharacterInfo)
	{
		aQuestionData.myCharacterInfo = &myCharacterInfo;
		return true;
	}

	return false;
}
