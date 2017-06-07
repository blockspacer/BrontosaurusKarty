#pragma once

enum class eCharacter
{
	eYoshi,
	eMario
};

struct SPlacementData
{
	unsigned char placement;
	eCharacter character;
	float time;
	bool isPlayer;
};