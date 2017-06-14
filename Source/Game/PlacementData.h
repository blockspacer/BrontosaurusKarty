#pragma once
#include "SParticipant.h"

struct SPlacementData
{
	unsigned char placement;
	SParticipant::eCharacter character;
	int minutesPassed;
	int secondsPassed;
	int hundredthsSecondsPassed;
	bool isPlayer;
	unsigned char inputDevice;
};