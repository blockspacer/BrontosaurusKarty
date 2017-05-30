#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <Renderer.h>
#include <StopWatch.h>
#include <EInputReturn.h>
#include <Scene.h>
#include <ParticleEmitterManager.h>

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
#include "RedShellManager.h"
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
#include "LightComponentManager.h"
#include "BrontosaurusEngine/SpriteInstance.h"
#include "ThreadedPostmaster/GameEventMessage.h"
#include <LuaWrapper/SSlua/SSlua.h>
#include <GUIElement.h>
#include "HUD.h"
#include "PollingStation.h"

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
#include "DriftTurner.h"
#include "HazardComponent.h"

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
	, myCameraComponents(4)
	, myPlayerCount(1)
	, myLevelIndex(aLevelIndex)
	, myIsLoaded(false)
	, myCountdownShouldRender(false)
{
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
		myKartObjects.Init(8);
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

	CLightComponentManager::Destroy();
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
	SAFE_DELETE(myItemBehaviourManager);
	CLapTrackerComponentManager::DestoyInstance();
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
		CreatePlayer(myScene->GetPlayerCamera(i).GetCamera(), myPlayers[i].myInputDevice, myPlayerCount);
	}

	for (int i = myPlayerCount; i < 8 - myPlayerCount; ++i)
	{
		CreateAI();
	}

	///////////////////
	//     HUD 

	myHUDs.Init(myPlayerCount);

	bool myIsOneSplit = false;
	if (myPlayerCount == 2)
	{
		myIsOneSplit = true;
	}
	for (int i = 0; i < myPlayerCount; ++i)
	{
		myHUDs.Add(new CHUD(i, myIsOneSplit));
		myHUDs[i]->LoadHUD();
	}

	myCountdownSprite->Render();

	///////////
	myIsLoaded = true;

	// Get time to load the level:
	loadPlaystateTimer.Update();
	float time = loadPlaystateTimer.GetDeltaTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();

}

void CPlayState::Init()
{
	for (int i = 0; i < myHUDs.Size(); ++i)
	{
		POSTMASTER.Subscribe(myHUDs[i], eMessageType::eCharPressed);
		POSTMASTER.Subscribe(myHUDs[i], eMessageType::eRaceOver);
	}


	myGameObjectManager->SendObjectsDoneMessage();
	CLapTrackerComponentManager::GetInstance()->Init();
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
	myPlayerControllerManager->Update(aDeltaTime.GetSeconds());

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
		myRespawnComponentManager->Update(aDeltaTime.GetSeconds());
	}
	if(CLapTrackerComponentManager::GetInstance() != nullptr)
	{
		CLapTrackerComponentManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	}

	if (myItemBehaviourManager != nullptr)
	{
		myItemBehaviourManager->Update(aDeltaTime.GetSeconds());
	}

	myRedShellManager->Update(aDeltaTime.GetSeconds());

	for (int i = 0; i < myPlayerCount; ++i)
	{
		myHUDs[i]->Update();
	}

	CPickupComponentManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	return myStatus;
}

void CPlayState::Render()
{
	myScene->RenderSplitScreen(myPlayerCount);

	if (myCountdownShouldRender)
		RenderCountdown();

	for (int i = 0; i < myPlayerCount; ++i)
	{
		myHUDs[i]->Render();
	}
}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);

	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/LevelList.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("levels");

	const char* song = levelsArray.at(myLevelIndex).GetString().c_str();

	Audio::CAudioInterface::GetInstance()->PostEvent(song);

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

	CLightComponentManager::Create(*myScene);
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
	myRedShellManager = new CRedShellManager();
	myRedShellManager->Init(myPhysicsScene, myKartControllerComponentManager,myKartObjects);
	myItemFactory = new CItemFactory();
	myItemFactory->Init(*myGameObjectManager, *myItemBehaviourManager, myPhysicsScene, *myColliderComponentManager,*myRedShellManager);
	myRespawnComponentManager = new CRespawnComponentManager();
	CLapTrackerComponentManager::CreateInstance();
	CKartSpawnPointManager::GetInstance()->Create();
	CPickupComponentManager::Create();
}

void CPlayState::LoadNavigationSpline(const CU::CJsonValue& splineData)
{
	myKartControllerComponentManager->LoadNavigationSpline(splineData);
}

void CPlayState::CreatePlayer(CU::Camera& aCamera, const SParticipant::eInputDevice aIntputDevice, unsigned int aPlayerCount)
{
	//Create sub sub player object
	CGameObject* secondPlayerObject = myGameObjectManager->CreateGameObject();
	CModelComponent* playerModel = myModelComponentManager->CreateComponent("Models/Animations/M_Kart_01.fbx");

	secondPlayerObject->AddComponent(playerModel);
	secondPlayerObject->AddComponent(new Component::CKartModelComponent(myPhysicsScene));
	//Create sub player object
	CGameObject* intermediary = myGameObjectManager->CreateGameObject();
	intermediary->AddComponent(new Component::CDriftTurner);
	intermediary->AddComponent(secondPlayerObject);
	//Create top player object
	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->AddComponent(intermediary);

	CU::Matrix44f kartTransformation = CKartSpawnPointManager::GetInstance()->PopSpawnPoint().mySpawnTransformaion;
	playerObject->SetWorldTransformation(kartTransformation);
	playerObject->Move(CU::Vector3f::UnitY);
	CCameraComponent* cameraComponent = new CCameraComponent(aPlayerCount);
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(aCamera);

	CRespawnerComponent* respawnComponent = myRespawnComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(respawnComponent);

	if(CLapTrackerComponentManager::GetInstance() != nullptr)
	{
		CLapTrackerComponent* lapTrackerComponent = CLapTrackerComponentManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(lapTrackerComponent);
	}

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

	CHazardComponent* hazardComponent = new CHazardComponent();
	hazardComponent->SetToPermanent();
	CComponentManager::GetInstance().RegisterComponent(hazardComponent);
	playerObject->AddComponent(hazardComponent);

	CItemHolderComponent* itemHolder = new CItemHolderComponent(*myItemFactory);
	CComponentManager::GetInstance().RegisterComponent(itemHolder);
	playerObject->AddComponent(itemHolder);

	playerObject->AddComponent(kartComponent);
	SBoxColliderData box;
	box.myHalfExtent = CU::Vector3f(1.0f, 1.0f, 1.0f);
	box.center.y = 1.05f;
	box.myLayer = Physics::eKart;
	box.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);

	SBoxColliderData triggerbox;
	triggerbox.myHalfExtent = CU::Vector3f(1.0f, 1.0f, 1.0f);
	triggerbox.center.y = 1.05f;
	triggerbox.myLayer = Physics::eKart;
	triggerbox.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);
	triggerbox.IsTrigger = true;

	CColliderComponent* playerColliderComponent = myColliderComponentManager->CreateComponent(&box, playerObject->GetId());
	CColliderComponent* playerTriggerColliderComponent = myColliderComponentManager->CreateComponent(&triggerbox, playerObject->GetId());

	
	SRigidBodyData rigidbodah;
	rigidbodah.isKinematic = true;
	rigidbodah.useGravity = false;
	rigidbodah.myLayer = Physics::eKart;
	rigidbodah.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);
	CColliderComponent* rigidComponent = myColliderComponentManager->CreateComponent(&rigidbodah, playerObject->GetId());
//	colliderObject->SetWorldPosition({ offset.x, offset.y + 0.1f, offset.z });
	playerObject->AddComponent(playerColliderComponent);
	playerObject->AddComponent(playerTriggerColliderComponent);
	playerObject->AddComponent(rigidComponent);


	playerObject->AddComponent(cameraComponent);
	myCameraComponents.Add(cameraComponent);


	CPollingStation::GetInstance()->AddPlayer(playerObject);

	//playerObject->Move(CU::Vector3f(0, 10, 0));

	myKartObjects.Add(playerObject);
}

void CPlayState::CreateAI()
{
	CGameObject* secondPlayerObject = myGameObjectManager->CreateGameObject();
	CModelComponent* playerModel = myModelComponentManager->CreateComponent("Models/Animations/M_Kart_01.fbx");

	secondPlayerObject->AddComponent(playerModel);
	secondPlayerObject->AddComponent(new Component::CKartModelComponent(myPhysicsScene));

	CGameObject* intermediary = myGameObjectManager->CreateGameObject();
	intermediary->AddComponent(new Component::CDriftTurner);
	intermediary->AddComponent(secondPlayerObject);

	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->AddComponent(intermediary);

	CU::Matrix44f kartTransformation = CKartSpawnPointManager::GetInstance()->PopSpawnPoint().mySpawnTransformaion;
	playerObject->SetWorldTransformation(kartTransformation);
	playerObject->Move(CU::Vector3f::UnitY);

	CRespawnerComponent* respawnComponent = myRespawnComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(respawnComponent);

	if (CLapTrackerComponentManager::GetInstance() != nullptr)
	{
		CLapTrackerComponent* lapTrackerComponent = CLapTrackerComponentManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(lapTrackerComponent);
	}

	CKartControllerComponent* kartComponent = myKartControllerComponentManager->CreateAndRegisterComponent();

	myPlayerControllerManager->CreateAIController(*kartComponent);

	if (CSpeedHandlerManager::GetInstance() != nullptr)
	{
		CSpeedHandlerComponent* speedHandlerComponent = CSpeedHandlerManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(speedHandlerComponent);
	}

	CHazardComponent* hazardComponent = new CHazardComponent();
	hazardComponent->SetToPermanent();
	CComponentManager::GetInstance().RegisterComponent(hazardComponent);
	playerObject->AddComponent(hazardComponent);

	CItemHolderComponent* itemHolder = new CItemHolderComponent(*myItemFactory);
	CComponentManager::GetInstance().RegisterComponent(itemHolder);
	playerObject->AddComponent(itemHolder);

	playerObject->AddComponent(kartComponent);
	SBoxColliderData box;
	box.myHalfExtent = CU::Vector3f(1.0f, 1.0f, 1.0f);
	box.center.y = 1.05f;
	box.myLayer = Physics::eKart;
	box.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);

	SBoxColliderData triggerbox;
	triggerbox.myHalfExtent = CU::Vector3f(1.0f, 1.0f, 1.0f);
	triggerbox.center.y = 1.05f;
	triggerbox.myLayer = Physics::eKart;
	triggerbox.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);
	triggerbox.IsTrigger = true;

	CColliderComponent* playerColliderComponent = myColliderComponentManager->CreateComponent(&box, playerObject->GetId());
	CColliderComponent* playerTriggerColliderComponent = myColliderComponentManager->CreateComponent(&triggerbox, playerObject->GetId());


	SRigidBodyData rigidbodah;
	rigidbodah.isKinematic = true;
	rigidbodah.useGravity = false;
	rigidbodah.myLayer = Physics::eKart;
	rigidbodah.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);
	CColliderComponent* rigidComponent = myColliderComponentManager->CreateComponent(&rigidbodah, playerObject->GetId());
	//	colliderObject->SetWorldPosition({ offset.x, offset.y + 0.1f, offset.z });
	playerObject->AddComponent(playerColliderComponent);
	playerObject->AddComponent(playerTriggerColliderComponent);
	playerObject->AddComponent(rigidComponent);


	myKartObjects.Add(playerObject);
}

void CPlayState::InitiateRace()
{
	auto countdownLambda = [this]() {

		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();
		unsigned char startCountdownTime = 0;
		float floatTime = 0.f;

		myCountdownShouldRender = true;

		while (floatTime <= 3.5)
		{
			timerManager.UpdateTimers();
			floatTime = timerManager.GetTimer(timer).GetLifeTime().GetSeconds();

			if ((unsigned char)floatTime > startCountdownTime)
			{
				startCountdownTime = std::floor(floatTime);

				if		(startCountdownTime == 0) 	myCountdownSprite->SetRect({ 0.f,0.75f,1.f,1.00f });
				else if (startCountdownTime == 1) 	myCountdownSprite->SetRect({ 0.f,0.50f,1.f,0.75f });
				else if (startCountdownTime == 2) 	myCountdownSprite->SetRect({ 0.f,0.25f,1.f,0.50f });
				else if (startCountdownTime == 3)
				{
					myCountdownSprite->SetRect({ 0.f,0.00f,1.f,0.25f });
					myKartControllerComponentManager->ShouldUpdate(true);
				}
			}
		}
	};

	CU::Work work(countdownLambda);
	work.SetName("Countdown thread");

	std::function<void(void)> callback = [this]() 
	{ 
		CU::TimerManager timerManager;
		TimerHandle timer = timerManager.CreateTimer();

		while (timerManager.GetTimer(timer).GetLifeTime().GetSeconds() < .05f)
		{
			timerManager.UpdateTimers(); // för att komma runt att trippelbuffringen slänger bort meddelanden.
			myCountdownSprite->SetAlpha(0);
		}

		myCountdownShouldRender = false;

	};

	work.SetFinishedCallback(callback);
	CU::ThreadPool::GetInstance()->AddWork(work);
}

void CPlayState::RenderCountdown()
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
