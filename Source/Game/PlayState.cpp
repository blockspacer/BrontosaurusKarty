#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <Renderer.h>
#include <StopWatch.h>
#include <EInputReturn.h>
#include <Scene.h>

#include "StateStack/StateStack.h"

//#include "Skybox.h"
#include "../Audio/AudioInterface.h"

#include "LoadState.h"
#include "ThreadedPostmaster/LoadLevelMessage.h"

//Managers and components

#include "ModelComponentManager.h"
#include "Components/ScriptComponentManager.h"

#include "LoadManager/LoadManager.h"
#include "KevinLoader/KevinLoader.h"

#include "CameraComponent.h"
#include "KartComponentManager.h"
#include "SpeedHandlerManager.h"

//Networking
#include "TClient/Client.h"
#include "TShared/NetworkMessage_ClientReady.h"
#include "TClient/ClientMessageManager.h"
#include "PostMaster/SendNetworkMessage.h"
#include "PostMaster/MessageType.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/PostOffice.h"
#include "ThreadedPostmaster/SendNetowrkMessageMessage.h"


#include "CommonUtilities/InputMessage.h"
#include <CommonUtilities/EKeyboardKeys.h>



//PHYSICS
#include "../Physics/Foundation.h"
#include "../Physics/Physics.h"
#include "../Physics/PhysicsScene.h"
#include "../Physics/PhysicsActorDynamic.h"
#include "ColliderComponentManager.h"
#include "BoxColliderComponent.h"
#include "Physics/PhysicsCharacterController.h"
#include "CharacterControllerComponent.h"
#include "../Components/ParticleEmitterComponentManager.h"
#include "ConcaveMeshCollider.h"

//Other stuff I dunno
#include "PointLightComponentManager.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "ThreadedPostmaster/GameEventMessage.h"
#include <LuaWrapper/SSlua/SSlua.h>

// player creationSpeciifcIncludes
#include "KartComponent.h"
#include "KeyboardControllerComponent.h"
#include "KartSpawnPointManager.h"
#include "XboxControllerComponent.h"
#include "KartControllerComponentManager.h"
#include "PlayerControllerManager.h"
#include "KartControllerComponent.h"
#include "PlayerController.h"
#include "SpeedHandlerComponent.h"

CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myPhysicsScene(nullptr)
	, myPhysics(nullptr)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myModelComponentManager(nullptr)
	, myColliderComponentManager(nullptr)
	, myScriptComponentManager(nullptr)
	, myCameraComponent(nullptr)
	, myLevelIndex(aLevelIndex)
	, myIsLoaded(false)
{
}

CPlayState::~CPlayState()
{
	CParticleEmitterComponentManager::Destroy();


	SAFE_DELETE(myScene);

	SAFE_DELETE(myModelComponentManager);
	SAFE_DELETE(myScriptComponentManager);

	CPointLightComponentManager::Destroy();
	CComponentManager::DestroyInstance();
	SAFE_DELETE(myColliderComponentManager);
	SAFE_DELETE(myPhysicsScene);
	SAFE_DELETE(myGameObjectManager);
	SAFE_DELETE(myKartComponentManager);
	CSpeedHandlerManager::Destroy();
	SAFE_DELETE(myKartControllerComponentManager);
	SAFE_DELETE(myPlayerControllerManager);
}

void CPlayState::Load()
{
	CU::CStopWatch loadPlaystateTimer;
	loadPlaystateTimer.Start();

	srand(static_cast<unsigned int>(time(nullptr)));

	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/LevelList.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	//**************************************************************//
	//							PHYSICS								//
	//**************************************************************//
	if (Physics::CFoundation::GetInstance() == nullptr)
	{
		Physics::CFoundation::Create();
		if (Physics::CFoundation::GetInstance()->GetPhysics() == nullptr)
		{
			myPhysics = Physics::CFoundation::GetInstance()->CreatePhysics();
		}
	}
	else
	{
		myPhysics = Physics::CFoundation::GetInstance()->GetPhysics();
	}

	if (myPhysicsScene != nullptr)
	{
		delete myPhysicsScene;
	}

	myPhysicsScene = myPhysics->CreateScene();

	//**************************************************************//
	//						END OF PHYSICS							//
	//**************************************************************//
	CU::CJsonValue levelsArray = levelsFile.at("levels");
	if (!levelsArray.HasIndex(myLevelIndex))
	{
		DL_MESSAGE_BOX("Tried to load level with index out of range (%d), level count is %d", myLevelIndex, levelsArray.Size());
		return;
	}

	std::string levelPath = "Json/Levels/";
	levelPath += levelsArray[myLevelIndex].GetString();
	levelPath += "/LevelData.json";


	CreateManagersAndFactories();
	LoadManagerGuard loadManagerGuard(*this, *myScene);

	//real loading:		as opposed to fake loading
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}

	Lights::SDirectionalLight dirLight;
	dirLight.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	dirLight.shadowIndex = 0;
	myScene->AddDirectionalLight(dirLight);
	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CRenderCamera& playerCamera = myScene->GetRenderCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.InitPerspective(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 0.1f, 500.f);


	CreatePlayer(playerCamera.GetCamera());
	myScene->SetSkybox("default_cubemap.dds");
	myScene->SetCubemap("purpleCubemap.dds");


	myIsLoaded = true;

	// Get time to load the level:
	loadPlaystateTimer.Update();
	float time = loadPlaystateTimer.GetDeltaTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();

}

void CPlayState::Init()
{
	myGameObjectManager->SendObjectsDoneMessage();
}

eStateStatus CPlayState::Update(const CU::Time& aDeltaTime)
{
	CParticleEmitterComponentManager::GetInstance().UpdateEmitters(aDeltaTime);
	CParticleEmitterManager::GetInstance().Update(aDeltaTime);
	myScene->Update(aDeltaTime);
	if (myPhysicsScene->Simulate(aDeltaTime) == true)
	{
		myColliderComponentManager->Update();
	}

	myKartComponentManager->Update(aDeltaTime.GetSeconds());
	myKartControllerComponentManager->Update(aDeltaTime.GetSeconds());
	
	if(myCameraComponent != nullptr)
	{
		myCameraComponent->Update(aDeltaTime.GetSeconds());
	}

	if (CSpeedHandlerManager::GetInstance() != nullptr)
	{
		CSpeedHandlerManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	}

	return myStatus;
}

void CPlayState::Render()
{
	myScene->Render();
}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
}

void CPlayState::OnExit(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
	RENDERER.ClearGui();
}

CU::eInputReturn CPlayState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed && aInputMessage.myKey == CU::eKeys::ESCAPE)
	{
		//myStateStack.PushState(new CPauseMenuState(myStateStack));
		return CU::eInputReturn::eKeepSecret;
	}

	return CU::CInputMessenger::RecieveInput(aInputMessage);
}

CGameObjectManager* CPlayState::GetGameObjectManager()
{
	return myGameObjectManager;
}

eMessageReturn CPlayState::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	myStateStack.SwapState(new CLoadState(myStateStack, aLoadLevelMessage.myLevelIndex));
	return eMessageReturn::eContinue;
}

void CPlayState::CreateManagersAndFactories()
{
	CComponentManager::CreateInstance();

	CParticleEmitterComponentManager::Create();

	myScene = new CScene();

	CPointLightComponentManager::Create(*myScene);
	myGameObjectManager = new CGameObjectManager();
	myModelComponentManager = new CModelComponentManager(*myScene);

	myColliderComponentManager = new CColliderComponentManager();
	myColliderComponentManager->SetPhysicsScene(myPhysicsScene);
	myColliderComponentManager->SetPhysics(myPhysics);
	myColliderComponentManager->InitControllerManager();

	myScriptComponentManager = new CScriptComponentManager();
	myKartComponentManager = new CKartComponentManager();
	CSpeedHandlerManager::CreateInstance();
	CSpeedHandlerManager::GetInstance()->Init();
	myKartControllerComponentManager = new CKartControllerComponentManager;
	myKartControllerComponentManager->Init(myPhysicsScene);
	myPlayerControllerManager = new CPlayerControllerManager;
	CKartSpawnPointManager::GetInstance().Create();
}

void CPlayState::CreatePlayer(CU::Camera& aCamera)
{
	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->Move({ 0.f, 1.f, 0.f });
	CModelComponent* playerModel = myModelComponentManager->CreateComponent("Models/Meshes/M_Kart_01.fbx");
	CCameraComponent* cameraComponent = myCameraComponent;
	cameraComponent = new CCameraComponent();
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(aCamera);
	myCameraComponent = cameraComponent;

	CKartControllerComponent* kartComponent = myKartControllerComponentManager->CreateAndRegisterComponent();
	CPlayerController* controls = myPlayerControllerManager->CreatePlayerController(*kartComponent);
	if(CSpeedHandlerManager::GetInstance() != nullptr)
	{
		CSpeedHandlerComponent* speedHandlerComponent = CSpeedHandlerManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(speedHandlerComponent);
	}
	/*CKartComponent* kartComponent = myKartComponentManager->CreateComponent();
	CKeyboardControllerComponent* keyBoardInput = new CKeyboardControllerComponent();
	CXboxControllerComponent* xboxInput = new CXboxControllerComponent();
	Subscribe(*keyBoardInput);
	Subscribe(*xboxInput);*/
	Subscribe(*controls);

	playerObject->AddComponent(playerModel);

	playerObject->AddComponent(kartComponent);
	/*SConcaveMeshColliderData crystalMeshColliderData;
	crystalMeshColliderData.IsTrigger = false;
	crystalMeshColliderData.myPath = "Models/Meshes/M_Kart_01.fbx";
	crystalMeshColliderData.material.aDynamicFriction = 0.5f;
	crystalMeshColliderData.material.aRestitution = 0.5f;
	crystalMeshColliderData.material.aStaticFriction = 0.5f;
	CColliderComponent* playerColliderComponent = myColliderComponentManager->CreateComponent(&crystalMeshColliderData, playerObject->GetId());
	playerObject->AddComponent(playerColliderComponent);*/
	SRigidBodyData rigidBodyData;
	//CColliderComponent* rigidBodyComponent = myColliderComponentManager->CreateComponent(&rigidBodyData, playerObject->GetId());
	//playerObject->AddComponent(rigidBodyComponent);
	//playerObject->AddComponent(kartComponent);
	//playerObject->AddComponent(keyBoardInput);
	//playerObject->AddComponent(xboxInput);

	playerObject->AddComponent(cameraComponent);
}

void CPlayState::SetCameraComponent(CCameraComponent* aCameraComponent)
{
	myCameraComponent = aCameraComponent;
}
