#pragma once



namespace Render
{
	struct SEffectData;
	namespace Effect
	{
		namespace EffectFactory
		{
			CEffect* CreateEffect(const SEffectData& anEffectData);
		}
	}
}
