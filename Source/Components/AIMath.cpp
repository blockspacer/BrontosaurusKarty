#include "stdafx.h"
#include "AIMath.h"

namespace AIMath
{
	CU::Vector2f pDistance(const float x, const float y, const float x1, const float y1, const float x2, const float y2)
	{
		float A = x - x1;
		float B = y - y1;
		float C = x2 - x1;
		float D = y2 - y1;

		float dot = A * C + B * D;
		float len_sq = C * C + D * D;
		float param = -1;
		if (len_sq != 0) //in case of 0 length line
			param = dot / len_sq;

		float xx, yy;

		if (param < 0) {
			xx = x1;
			yy = y1;
		}
		else if (param > 1) {
			xx = x2;
			yy = y2;
		}
		else {
			xx = x1 + param * C;
			yy = y1 + param * D;
		}

		return CU::Vector2f(xx, yy);
	}

	float signof(float aFloat)
	{
		if (aFloat < 0.f)
		{
			return -1.f;
		}
		return 1.f;
	}
}
