#pragma once

namespace Physics
{
	static const unsigned int CollideEverything = ~0u;
	enum ECollisionLayer : unsigned int
	{
		eDefault = 1 << 0,
		// 1 - 7 unity reserved idk
		eKart = 1 << 8,
		eItemBox = 1 << 9,
		eBoostpad = 1 << 10,
		eGround = 1 << 11,
		eHazzard = 1 << 12,

		eEverything = CollideEverything,
	};


	inline ECollisionLayer GetLayerFromUnity(const unsigned int aLayer)
	{
		assert(aLayer >= 0 && aLayer <= eGround);
		return static_cast<ECollisionLayer>(1 << aLayer);
	}

	inline unsigned int GetCollideAgainst(const ECollisionLayer aLayer)
	{
		switch (aLayer)
		{
		case Physics::eDefault:
			return CollideEverything;
		case Physics::eKart:
			return eDefault | eItemBox | eBoostpad | eKart | eHazzard;
		case Physics::eItemBox:
			return eKart;
		case Physics::eBoostpad:
			return eKart;
		case Physics::eGround:
			return 0;
		case Physics::eHazzard:
			return eKart | eHazzard;
		default:
			return CollideEverything;
		}	
	}
}
