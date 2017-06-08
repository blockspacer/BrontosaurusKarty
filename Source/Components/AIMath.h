#pragma once

namespace AIMath
{
	constexpr float maxAngleInv = 1.0f / (2.f * 3.1415f);

	CU::Vector2f pDistance(const float x, const float y, const float x1, const float y1, const float x2, const float y2);

	float signof(float aFloat);

	template<typename T, typename U>
	bool samesign(const T aLeft, const U aRight)
	{
		if (aLeft > 0)
		{
			if (aRight > 0)
			{
				return true;
			}
			return false;
		}
		if (aRight < 0)
		{
			return true;
		}
	}
}
