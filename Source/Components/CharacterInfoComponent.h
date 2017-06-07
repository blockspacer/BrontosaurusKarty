#pragma once
#include "SParticipant.h"

struct SCharacterInfo
{
	SParticipant::eCharacter characterType;
	bool isAI;
};

class CCharacterInfoComponent : public CComponent
{
public:
	CCharacterInfoComponent(SParticipant::eCharacter aCharacterType, bool aIsAI);
	~CCharacterInfoComponent();

	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
private:
	SCharacterInfo myCharacterInfo;
};

