#include "stdafx.h"
#include "LoadManager.h"
#include "DL_Debug.h"
#include "LoadObject.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadModel.h"
#include "LoadCamera.h"
#include "LoadSoundComponent.h"
#include "LoadScriptComponent.h"
#include "LoadFireComponent.h"
#include "LoadPointLightComponent.h"
#include "LoadCollider.h"

#include "LoadParticleEmitterComponent.h"
#include "LoadHighlightComponent.h"

LoadManager* LoadManager::ourInstance = nullptr;

void LoadManager::CreateInstance(CPlayState& aPlayState, CScene& aScene)
{
	assert(ourInstance == nullptr && "Load manager already created");
	ourInstance = new LoadManager(aPlayState, aScene);
}

LoadManager* LoadManager::GetInstance()
{
	return ourInstance;
}

void LoadManager::DestroyInstance()
{
	assert(ourInstance != nullptr && "Load manager not created (is NULL)");
	SAFE_DELETE(ourInstance);
}

CScene& LoadManager::GetCurrentScene() const
{
	return myCurrentScene;
}

CPlayState& LoadManager::GetCurrentPLaystate() const
{
	return myCurrentPlaystate;
}

void LoadManager::RegisterFunctions()
{
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	loader.RegisterObjectLoadFunction(LoadObject);
	loader.RegisterObjectLinkFunction(LinkObject);

	loader.RegisterComponentLoadFunction("MeshFilter", LoadMeshFilter);
	loader.RegisterComponentLoadFunction("Camera", LoadCamera);
	loader.RegisterComponentLoadFunction("ParticleEmitterComponent", LoadParticleEmitterComponent);
	loader.RegisterComponentLoadFunction("BoxCollider", LoadBoxCollider);
	loader.RegisterComponentLoadFunction("SphereCollider", LoadSphereCollider);
	loader.RegisterComponentLoadFunction("CapsuleCollider", LoadCapsuleCollider);
	loader.RegisterComponentLoadFunction("MeshCollider", LoadMeshCollider);
	loader.RegisterComponentLoadFunction("Rigidbody", LoadRigidBody);
	loader.RegisterComponentLoadFunction("CharacterController", LoadCharacterController);
	//loader.RegisterComponentLoadFunction("ScriptComponent", LoadScriptComponent);
	loader.RegisterComponentLoadFunction("HighlightComponent",LoadHighlightComponent);
	loader.RegisterComponentLoadFunction("Light", LoadPointLightComponent);

	//loader.RegisterComponentLoadFunction("CircleCollider", LoadCircleCollider);
	//loader.RegisterComponentLoadFunction("SoundComponent", LoadSoundComponent);
	//loader.RegisterComponentLoadFunction("FireComponent", LoadFireComponent);
}

LoadManager::LoadManager(CPlayState& aPlayState, CScene& aScene)
	: myCurrentPlaystate(aPlayState)
	, myCurrentScene(aScene)
{
	RegisterFunctions();
}


LoadManager::~LoadManager()
{
}
