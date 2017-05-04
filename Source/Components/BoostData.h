#pragma once

enum class eBoostType
{
	eNoBoost,
	eDefault,
	eBadTerrainSpeedDecrease
};
struct SBoostData
{
	float maxSpeedBoost;
	float accerationBoost;
	float duration;
	eBoostType type;
};