#include "stdafx.h"
#include "CharacterInfoComponent.h"


CCharacterInfoComponent::CCharacterInfoComponent(SParticipant::eCharacter aCharacterType, bool aIsAI, unsigned char aInputDevice)
{
	myCharacterInfo.characterType = aCharacterType;
	myCharacterInfo.isAI = aIsAI;
	myCharacterInfo.inputDevice = aInputDevice;
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
