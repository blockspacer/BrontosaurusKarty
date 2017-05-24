#pragma once

namespace Physics
{
	static const unsigned int CollideEverything = ~0u;
	enum ECollisionLayer : unsigned int
	{
		eNone = 0,
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

	inline ECollisionLayer GetCollideAgainst(const ECollisionLayer aLayer)
	{
		switch (aLayer)
		{
		case Physics::eDefault:
			return eEverything;
		case Physics::eKart:
			return static_cast<ECollisionLayer>(eDefault | eItemBox | eBoostpad | eKart | eHazzard);
		case Physics::eItemBox:
			return eKart;
		case Physics::eBoostpad:
			return eKart;
		case Physics::eGround:
			return eNone;
		case Physics::eHazzard:
			return static_cast<ECollisionLayer>(eKart | eHazzard);
		default:
			return eEverything;
		}	
	}
}
