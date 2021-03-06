#pragma once

#include "../CommonUtilities/BitSet.h"

enum class eAnchors
{
	eTop,
	eBottom,
	eLeft,
	eRight
};

struct SGUIElement
{
	CU::Vector4f myScreenRect;
	CU::Vector2f myOrigin;
	CU::CBitSet<4> myAnchor;
};
