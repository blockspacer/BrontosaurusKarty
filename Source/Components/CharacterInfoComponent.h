#pragma once
#include "SParticipant.h"

struct SCharacterInfo
{
	SParticipant::eCharacter characterType;
	unsigned char inputDevice;
	bool isAI;
};

class CCharacterInfoComponent : public CComponent
{
public:
	CCharacterInfoComponent(SParticipant::eCharacter aCharacterType, bool aIsAI, unsigned char aInputDevice);
	~CCharacterInfoComponent();

	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
private:
	SCharacterInfo myCharacterInfo;
};

