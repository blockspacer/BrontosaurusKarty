#include "stdafx.h"
#include "EffectsFactory.h"
#include "EffectsManager.h"
#include "ShaderManager.h"
#include "EModelBluePrint.h"

const std::wstring defaultVertexShader = L"Shaders/vertex_shader.fx";
const std::wstring defaultPixelShader = L"Shaders/Deferred/deferred_pixel.fx";
namespace Render
{
	namespace Effect
	{
		namespace EffectFactory
		{
			ID3D11VertexShader* CreateVertexShader(const SEffectData& aEffectData);
			ID3D11PixelShader* CreatePixelShader(const SEffectData& aEffectData);
			ID3D11GeometryShader* CreateGeometryShader(const SEffectData& aEffectData);
			ID3D11InputLayout* CreateInputLayout(const SEffectData& aEffectData);
			D3D_PRIMITIVE_TOPOLOGY GetTopology(const SEffectData& aEffectData);

			std::wstring GetPixelShaderFile(const Render::SEffectData& aEffectData);
		}
	}
}

CEffect* Render::Effect::EffectFactory::CreateEffect(const SEffectData& anEffectData)
{
	return new CEffect(Render::Effect::EffectFactory::CreateVertexShader(anEffectData),
		Render::Effect::EffectFactory::CreatePixelShader(anEffectData),
		Render::Effect::EffectFactory::CreateGeometryShader(anEffectData),
		Render::Effect::EffectFactory::CreateInputLayout(anEffectData),
		Render::Effect::EffectFactory::GetTopology(anEffectData)
		);
}

ID3D11VertexShader* Render::Effect::EffectFactory::CreateVertexShader(const SEffectData& aEffectData)
{
	if(aEffectData.vertexBlueprint == 0)
	{
		return nullptr;
	}
	return SHADERMGR->LoadVertexShader(defaultVertexShader, aEffectData.vertexBlueprint);
}



ID3D11PixelShader* Render::Effect::EffectFactory::CreatePixelShader(const SEffectData& aEffectData)
{
	if(aEffectData.pixelBlueprint == 0)
	{
		return nullptr;
	}
	return SHADERMGR->LoadPixelShader(GetPixelShaderFile(aEffectData),aEffectData.pixelBlueprint);
}

ID3D11GeometryShader* Render::Effect::EffectFactory::CreateGeometryShader(const SEffectData& aEffectData)
{
	if(aEffectData.geometryBlueprint == 0)
	{
		return nullptr;
	}

	return nullptr;
}

ID3D11InputLayout* Render::Effect::EffectFactory::CreateInputLayout(const SEffectData& aEffectData)
{
	if(aEffectData.vertexBlueprint == 0)
	{
		return nullptr;
	}
	return SHADERMGR->LoadInputLayout(defaultVertexShader, aEffectData.pixelBlueprint);
}

D3D_PRIMITIVE_TOPOLOGY Render::Effect::EffectFactory::GetTopology(const SEffectData& aEffectData)
{
	if(aEffectData.vertexBlueprint == 0)
	{
		return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

std::wstring Render::Effect::EffectFactory::GetPixelShaderFile(const Render::SEffectData& aEffectData)
{
	int pixelData = aEffectData.pixelBlueprint;

	if(pixelData & EModelBluePrint::EModelBluePrint_Deferred)
	{
		return L"Shaders/Deferred/deferred_pixel.fx";
	}

	return defaultPixelShader;
}
