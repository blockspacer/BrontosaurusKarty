#include "stdafx.h"
#include "EffectsManager.h"
#include "EffectsFactory.h"

Render::CEffectsManager* Render::CEffectsManager::ourInstance = nullptr;

Render::CEffectsManager::CEffectsManager()
{
	SEffectData effectData;

	GetEffect(effectData);
}


Render::CEffectsManager::~CEffectsManager()
{
	VertexShaderData::iterator vIt;
	for(vIt = myEffects.begin(); vIt != myEffects.end(); ++vIt)
	{
		GeometryShaderData::iterator gIt;
		for(gIt = vIt->second.begin(); gIt != vIt->second.end();++gIt)
		{
			PixelShaderData::iterator pIt;
			for(pIt = gIt->second.begin(); pIt != gIt->second.end(); ++pIt)
			{
				SAFE_DELETE(pIt->second);
			}
		}
	
	}
}

void Render::CEffectsManager::Create()
{
	if(ourInstance != nullptr)
	{
		DL_ASSERT("Effectsmanager has already been created.");
	}
	ourInstance = new CEffectsManager;
}

void Render::CEffectsManager::Destroy()
{
	if (ourInstance == nullptr)
	{
		DL_ASSERT("Effectsmanager is not created.");
	}
	SAFE_DELETE(ourInstance);
}

Render::CEffectsManager& Render::CEffectsManager::GetInstance()
{
	if (ourInstance == nullptr)
	{
		DL_ASSERT("Effectsmanager is not created.");
	}

	return *ourInstance;
}

CEffect* Render::CEffectsManager::GetEffect(const SEffectData& aEffectData)
{
	return GetEffectInternal(myEffects[aEffectData.vertexBlueprint][aEffectData.geometryBlueprint], aEffectData);
}

CEffect* Render::CEffectsManager::GetEffectInternal(VertexShaderData::mapped_type::mapped_type& aEffectsCollection, const SEffectData& aEffectData)
{
	VertexShaderData::mapped_type::mapped_type::iterator effectIt = aEffectsCollection.find(aEffectData.pixelBlueprint);

	if (effectIt == aEffectsCollection.end())
	{
		CEffect* effect = Effect::EffectFactory::CreateEffect(aEffectData);
		aEffectsCollection[aEffectData.pixelBlueprint] = effect;
		return effect;
	}

	return effectIt->second;
}


