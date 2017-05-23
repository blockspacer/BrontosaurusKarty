#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <Renderer.h>
#include <StopWatch.h>
#include <EInputReturn.h>
#include <Scene.h>

#include "StateStack.h"

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
#include "ItemHolderComponent.h"

#include "KartComponentManager.h"
#include "SpeedHandlerManager.h"
#include "BoostPadComponentManager.h"
#include "ItemFactory.h"
#include "../Components/PickupComponentManager.h"
#include "ItemWeaponBehaviourComponentManager.h"
#include "RespawnComponentManager.h"
#include "LapTrackerComponentManager.h"

//Networking
#include "ThreadedPostmaster/Postmaster.h"
#include "ThreadedPostmaster/PostOffice.h"

// Common Utilities
#include "CommonUtilities/InputMessage.h"
#include <CommonUtilities/EKeyboardKeys.h>
#include <ThreadPool.h>

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
#include <GUIElement.h>

// player creationSpeciifcIncludes
#include "KartComponent.h"
#include "KeyboardControllerComponent.h"
#include "KartSpawnPointManager.h"
#include "XboxControllerComponent.h"
#include "KartControllerComponentManager.h"
#include "PlayerControllerManager.h"
#include "KartControllerComponent.h"
#include "KeyboardController.h"
#include "SpeedHandlerComponent.h"
#include "XboxController.h"
#include "SmoothRotater.h"
#include "KartModelComponent.h"
#include "RespawnerComponent.h"
#include "LapTrackerComponent.h"
#include "BrontosaurusEngine/ParticleEmitterManager.h"

CPlayState::CPlayState(StateStack & aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myPhysicsScene(nullptr)
	, myPhysics(nullptr)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myModelComponentManager(nullptr)
	, myColliderComponentManager(nullptr)
	, myScriptComponentManager(nullptr)
	, myItemFactory(nullptr)
	, myRespawnComponentManager(nullptr)
	, myLapTrackerComponentManager(nullptr)
	, myCameraComponents(4)
	, myPlayerCount(1)
	, myLevelIndex(aLevelIndex)
	, myIsLoaded(false)
	, myCountdownShouldRender(false)
{
	myPlayers.Init(1);
	myPlayerCount = 1;
	myPlayers.Add(SParticipant());
	myPlayers[0].myInputDevice = SParticipant::eInputDevice::eKeyboard;
}

CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex, const CU::GrowingArray<SParticipant> aPlayers)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myPhysicsScene(nullptr)
	, myPhysics(nullptr)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myModelComponentManager(nullptr)
	, myColliderComponentManager(nullptr)
	, myScriptComponentManager(nullptr)
	, myItemFactory(nullptr)
	, myRespawnComponentManager(nullptr)
	, myLapTrackerComponentManager(nullptr)
	, myCameraComponents(4)
	, myPlayerCount(1)
	, myLevelIndex(aLevelIndex)
	, myIsLoaded(false)
	, myCountdownShouldRender(false)
{
	CommandLineManager* commandLineManager = CommandLineManager::GetInstance();
	if (commandLineManager)
	{
		const std::string& playerCountStr = commandLineManager->GetArgument("-playerCount");
		if (!playerCountStr.empty())
		{
			myPlayerCount = std::atoi(playerCountStr.c_str());
		}
	}
	if (aPlayers.Size() > 0)
	{
		myPlayers.Init(aPlayers.Size());
		myPlayerCount = aPlayers.Size();
		for (unsigned int i = 0; i < aPlayers.Size(); ++i)
		{
			myPlayers.Add(aPlayers[i]);
		}
	}
	else
	{
		myPlayers.Init(1);
		myPlayerCount = 1;
		myPlayers.Add(SParticipant());
		myPlayers[0].myInputDevice = SParticipant::eInputDevice::eKeyboard;
	}
	DL_PRINT("started with %d players", myPlayerCount);
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
	SAFE_DELETE(myBoostPadComponentManager);
	SAFE_DELETE(myItemFactory);
	SAFE_DELETE(myRespawnComponentManager);
	SAFE_DELETE(myLapTrackerComponentManager);
}

// Runs on its own thread.
void CPlayState::Load()
{
	CU::CStopWatch loadPlaystateTimer;
	loadPlaystateTimer.Start();

	myCountdownSprite = new CSpriteInstance("Sprites/GUI/countdown.dds");
	myCountdownSprite->SetPivot({ 0.5f,0.5f });
	myCountdownSprite->SetSize({ 0.25f,0.25f });
	myCountdownSprite->SetRect({ 0.f,0.75f,1.f,1.f });
	myCountdownSprite->SetPosition({ 0.5f,0.5f });

	myCountdownElement = new SGUIElement();
	myCountdownElement->myAnchor = (char)eAnchors::eTop | (char)eAnchors::eLeft;
	// Render in Renderfunc.



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


	//myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	//CRenderCamera& playerCamera = myScene->GetRenderCamera(CScene::eCameraType::ePlayerOneCamera);
	//playerCamera.InitPerspective(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 0.1f, 500.f);

	myScene->InitPlayerCameras(myPlayerCount);
	for (int i = 0; i < myPlayerCount; ++i)
	{
		CreatePlayer(myScene->GetPlayerCamera(i).GetCamera(),myPlayers[i].myInputDevice);
	}

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
	
	for (CCameraComponent* camera : myCameraComponents)
	{
		camera->Update(aDeltaTime.GetSeconds());
	}

	if (CSpeedHandlerManager::GetInstance() != nullptr)
	{
		CSpeedHandlerManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	}
	if (myBoostPadComponentManager != nullptr)
	{
		myBoostPadComponentManager->Update();
	}
	if(myRespawnComponentManager != nullptr)
	{
		myRespawnComponentManager->Update();
	}
	if(myLapTrackerComponentManager != nullptr)
	{
		myLapTrackerComponentManager->Update();
	}

	CPickupComponentManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	return myStatus;
}

void CPlayState::Render()
{
	myScene->RenderSplitScreen(myPlayerCount);

	if (myCountdownShouldRender)
	{
		SCreateOrClearGuiElement* createOrClear = new SCreateOrClearGuiElement(L"countdown", *myCountdownElement, CU::Vector2ui(WINDOW_SIZE.x, WINDOW_SIZE.y));
		RENDERER.AddRenderMessage(createOrClear);

		SChangeStatesMessage* const changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eAddBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		SRenderToGUI* const guiChangeState = new SRenderToGUI(L"countdown", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		myCountdownSprite->RenderToGUI(L"countdown");
	}
}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
	InitiateRace();
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
	myBoostPadComponentManager = new CBoostPadComponentManager();
	myItemBehaviourManager = new CItemWeaponBehaviourComponentManager();
	myItemBehaviourManager->Init(myPhysicsScene);
	myItemFactory = new CItemFactory();
	myItemFactory->Init(*myGameObjectManager, *myItemBehaviourManager, myPhysicsScene, *myColliderComponentManager);
	myRespawnComponentManager = new CRespawnComponentManager();
	myLapTrackerComponentManager = new CLapTrackerComponentManager();
	CKartSpawnPointManager::GetInstance()->Create();
	CPickupComponentManager::Create();
}

void CPlayState::LoadNavigationSpline(const CU::CJsonValue& splineData)
{
	myKartControllerComponentManager->LoadNavigationSpline(splineData);
}

void CPlayState::CreatePlayer(CU::Camera& aCamera, const SParticipant::eInputDevice aIntputDevice)
{
	//Create sub player object
	CGameObject* secondPlayerObject = myGameObjectManager->CreateGameObject();
	CModelComponent* playerModel = myModelComponentManager->CreateComponent("Models/Meshes/M_Kart_01.fbx");

	secondPlayerObject->AddComponent(playerModel);
	secondPlayerObject->AddComponent(new Component::CKartModelComponent(myPhysicsScene));

	//Create top player object
	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->AddComponent(secondPlayerObject);

	CU::Matrix44f kartTransformation = CKartSpawnPointManager::GetInstance()->PopSpawnPoint().mySpawnTransformaion;
	playerObject->SetWorldTransformation(kartTransformation);
	playerObject->Move(CU::Vector3f::UnitY);
	CCameraComponent* cameraComponent = new CCameraComponent();
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(aCamera);

	CRespawnerComponent* respawnComponent = myRespawnComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(respawnComponent);

	CLapTrackerComponent* lapTrackerComponent = myLapTrackerComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(lapTrackerComponent);

	CKartControllerComponent* kartComponent = myKartControllerComponentManager->CreateAndRegisterComponent();
	if (aIntputDevice == SParticipant::eInputDevice::eKeyboard)
	{
		CKeyboardController* controls = myPlayerControllerManager->CreateKeyboardController(*kartComponent);
		if (myPlayerCount < 2)
		{
			AddXboxController();
			CXboxController* xboxInput = myPlayerControllerManager->CreateXboxController(*kartComponent, static_cast<short>(SParticipant::eInputDevice::eController1));
		}
	}
	else
	{
		CXboxController* xboxInput = myPlayerControllerManager->CreateXboxController(*kartComponent, static_cast<short>(aIntputDevice));
	}
	if(CSpeedHandlerManager::GetInstance() != nullptr)
	{
		CSpeedHandlerComponent* speedHandlerComponent = CSpeedHandlerManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(speedHandlerComponent);
	}


	CItemHolderComponent* itemHolder = new CItemHolderComponent(*myItemFactory);
	playerObject->AddComponent(itemHolder);

	playerObject->AddComponent(kartComponent);
	SBoxColliderData box;
	box.myHalfExtent = CU::Vector3f(1.0f, 1.0f, 1.0f);
	box.center.y = 1.05f;
	SConcaveMeshColliderData crystalMeshColliderData;
	crystalMeshColliderData.IsTrigger = false;
	crystalMeshColliderData.myPath = "Models/Meshes/M_Kart_01.fbx";
	crystalMeshColliderData.material.aDynamicFriction = 0.5f;
	crystalMeshColliderData.material.aRestitution = 0.5f;
	crystalMeshColliderData.material.aStaticFriction = 0.5f;
	crystalMeshColliderData.myLayer;
	CColliderComponent* playerColliderComponent = myColliderComponentManager->CreateComponent(&box, playerObject->GetId());
	CGameObject* colliderObject = myGameObjectManager->CreateGameObject();
	CU::Vector3f offset = playerObject->GetWorldPosition();

	SRigidBodyData rigidbodah;
	rigidbodah.isKinematic = true;
	rigidbodah.useGravity = false;
	CColliderComponent* rigidComponent = myColliderComponentManager->CreateComponent(&rigidbodah, playerObject->GetId());
//	colliderObject->SetWorldPosition({ offset.x, offset.y + 0.1f, offset.z });
	colliderObject->AddComponent(playerColliderComponent);

	colliderObject->AddComponent(rigidComponent);
	playerObject->AddComponent(colliderObject);

	playerObject->AddComponent(cameraComponent);
	myCameraComponents.Add(cameraComponent);
}

void CPlayState::InitiateRace()
{
	auto countdownLambda = [this]() {

		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();
		float startCountdownTime = 0.f;


		myCountdownShouldRender = true;

		while (startCountdownTime < 4.f)
		{
			timerManager.UpdateTimers();
			float newTime = timerManager.GetTimer(timer).GetLifeTime().GetSeconds();
			newTime = std::floor(newTime);
			if (newTime > startCountdownTime)
			{
				startCountdownTime = newTime;

				if		(startCountdownTime == 0) 	myCountdownSprite->SetRect({ 0.f,0.75f,1.f,1.00f });
				else if (startCountdownTime == 1) 	myCountdownSprite->SetRect({ 0.f,0.50f,1.f,0.75f });
				else if (startCountdownTime == 2) 	myCountdownSprite->SetRect({ 0.f,0.25f,1.f,0.50f });
				else if (startCountdownTime == 3) 	myCountdownSprite->SetRect({ 0.f,0.00f,1.f,0.25f });
			}
		}

		while (timerManager.GetTimer(timer).GetLifeTime().GetSeconds() < 4.1f)
		{
			timerManager.UpdateTimers(); // ifrågasätt inte..
			myCountdownSprite->SetAlpha(0);

		}
	};

	CU::Work work(countdownLambda);
	work.SetName("Countdown thread");
	std::function<void(void)> callback = [this]() 
	{ 
		myKartControllerComponentManager->ShouldUpdate(true);
		myCountdownShouldRender = false;
	};
	work.SetFinishedCallback(callback);
	CU::ThreadPool::GetInstance()->AddWork(work);
}

void CPlayState::SetCameraComponent(CCameraComponent* aCameraComponent)
{
	DL_ASSERT("cant add camera component like this right now,,,,,,");
}
