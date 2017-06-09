#pragma once
#include "SParticipant.h"

struct SPlacementData
{
	unsigned char placement;
	SParticipant::eCharacter character;
	float time;
	bool isPlayer;
};