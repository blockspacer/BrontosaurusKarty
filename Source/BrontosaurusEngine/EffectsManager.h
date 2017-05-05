#pragma once
namespace Render
{
		struct SEffectData
		{
			int vertexBlueprint = 0;
			int geometryBlueprint = 0;
			int pixelBlueprint = 0;
		};
	class CEffectsManager
	{
	public:

		using PixelShaderData = std::map <int, CEffect*>;
		using GeometryShaderData = std::map<int, PixelShaderData>;

		using VertexShaderData = std::map<int, GeometryShaderData>;
		using EffectsCollection = VertexShaderData;

		static void Create();
		static void Destroy();
		static CEffectsManager& GetInstance();
		CEffect* GetEffect(const SEffectData& aEffectData);

	protected:
		static CEffectsManager* ourInstance;
		CEffect* GetEffectInternal(VertexShaderData::mapped_type::mapped_type& aEffectsCollection, const SEffectData& aEffectData);
		
		EffectsCollection myEffects;
		CEffectsManager();
		~CEffectsManager();
	};
}

