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
#include "BlueShellComponentManager.h"
#include "RespawnComponentManager.h"
#include "LapTrackerComponentManager.h"
#include "ExplosionComponentManager.h"

#include "AudioSourceComponent.h"
#include "AudioSourceComponentManager.h"

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
#include "..\ThreadedPostmaster\RaceStartedMessage.h"
#include <LuaWrapper/SSlua/SSlua.h>
#include <GUIElement.h>
#include "LocalHUD.h"
#include "GlobalHUD.h"
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
#include "AnimationEventFactory.h"
#include "..\CommonUtilities\JsonValue.h"
#include "CharacterInfoComponent.h"

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
	, myIsCountingDown(true)
{
	myPlayers.Init(1);
	myPlayerCount = 1;
	myPlayers.Add(SParticipant());
	myPlayers[0].myInputDevice = SParticipant::eInputDevice::eKeyboard;
	myPlacementLinesGUIElement.Init(8);
	myPlacementLineScreenSpaceWidth = 0.0f;
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
	,myIsCountingDown(true)
{
	if (aPlayers.Size() > 0)
	{
		myPlayers.Init(aPlayers.Size());
		myKartObjects.Init(8);
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
		myPlayers[0].myInputDevice = SParticipant::eInputDevice::eController1;
	}

	myPlacementLinesGUIElement.Init(8);
	myPlacementLineScreenSpaceWidth = 0.0f;

	if (CAnimationEventFactory::GetInstance() == nullptr)
	{
		new CAnimationEventFactory();
	}

	DL_PRINT("started with %d players", myPlayerCount);
}

CPlayState::~CPlayState()
{
	CParticleEmitterComponentManager::Destroy();
	CAudioSourceComponentManager::Destroy();

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

	CKartSpawnPointManager::GetInstance()->Destroy();
	CPickupComponentManager::Destroy();
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
	myCountdownElement->myAnchor.Set((unsigned int)eAnchors::eTop);
	myCountdownElement->myAnchor.Set((unsigned int)eAnchors::eLeft);
	myCountdownElement->myScreenRect.Set(0.f, 0.f, 1.f, 1.f);
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
	
	myScene->InitPlayerCameras(myPlayerCount);
	for (int i = 0; i < myPlayerCount; ++i)
	{
		CreatePlayer(myScene->GetPlayerCamera(i).GetCamera(), myPlayers[i], myPlayerCount);
	}

	for (int i = 0; i < 8 - myPlayerCount; ++i)
	{
		CreateAI();
	}

	///////////////////
	//     HUD 

	myLocalHUDs.Init(myPlayerCount);

	bool myIsOneSplit = false;
	if (myPlayerCount == 2)
	{
		myIsOneSplit = true;
	}
	for (int i = 0; i < myPlayerCount; ++i)
	{
		myLocalHUDs.Add(new CLocalHUD(i, myPlayerCount));
		myLocalHUDs[i]->LoadHUD();
	}

	myGlobalHUD = new CGlobalHUD();
	myGlobalHUD->LoadHUD();

	LoadPlacementLineGUI();

	myCountdownSprite->Render();

	//***************************************************************
	//*						BAKE SHADOWMAP							*
	//***************************************************************

	myScene->BakeShadowMap();
	RENDERER.SwapWrite();
	DL_PRINT("Baking Shadow Map");
	RENDERER.DoImportantQueue();
	{
		std::string progress = ".";
		while (!myScene->HasBakedShadowMap())
		{
			DL_PRINT(progress.c_str());
			progress += ".";
			std::this_thread::sleep_for(std::chrono::microseconds(7500));
		}
	}
	DL_PRINT("Done!");

	//--------------------------------------------------------------





	myIsLoaded = true;

	// Get time to load the level:
	loadPlaystateTimer.Update();
	float time = loadPlaystateTimer.GetDeltaTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
}

void CPlayState::Init()
{
	for (int i = 0; i < myLocalHUDs.Size(); ++i)
	{
		POSTMASTER.Subscribe(myLocalHUDs[i], eMessageType::eBlueShellWarning);
		POSTMASTER.Subscribe(myLocalHUDs[i], eMessageType::eCharPressed);
	}

	POSTMASTER.Subscribe(myGlobalHUD, eMessageType::eCharPressed);
	POSTMASTER.Subscribe(myGlobalHUD, eMessageType::eRaceOver);
	//POSTMASTER.Subscribe()

	POSTMASTER.Subscribe(myPlayerControllerManager, eMessageType::ePlayerFinished);
	POSTMASTER.Subscribe(myPlayerControllerManager, eMessageType::eRaceStarted);

	myGameObjectManager->SendObjectsDoneMessage();
	myKartControllerComponentManager->Init();
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
	myBlueShellManager->Update(aDeltaTime.GetSeconds());
	myExplosionManager->Update(aDeltaTime.GetSeconds());

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
		myLocalHUDs[i]->Render();
	}

	myGlobalHUD->Render();

	RenderPlacementLine();
}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);


	if (myIsCountingDown == false)
	{
		CU::CJsonValue levelsFile;
		std::string errorString = levelsFile.Parse("Json/LevelList.json");
		if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

		CU::CJsonValue levelsArray = levelsFile.at("levels");

		const char* song = levelsArray.at(myLevelIndex).GetString().c_str();

		Audio::CAudioInterface::GetInstance()->PostEvent(song);
	}

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

	CAudioSourceComponentManager::Create();

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
	myKartControllerComponentManager->SetPhysiscsScene(myPhysicsScene);
	myPlayerControllerManager = new CPlayerControllerManager;
	myBoostPadComponentManager = new CBoostPadComponentManager();
	myItemBehaviourManager = new CItemWeaponBehaviourComponentManager();
	myItemBehaviourManager->Init(myPhysicsScene);
	myExplosionManager = new CExplosionComponentManager;
	myRedShellManager = new CRedShellManager();
	myRedShellManager->Init(myPhysicsScene, myKartControllerComponentManager,myKartObjects);
	myBlueShellManager = new CBlueShellComponentManager(myKartObjects,myGameObjectManager,myExplosionManager);
	myItemFactory = new CItemFactory();
	myItemFactory->Init(*myGameObjectManager, *myItemBehaviourManager, myPhysicsScene, *myColliderComponentManager,*myRedShellManager,*myBlueShellManager);
	myRespawnComponentManager = new CRespawnComponentManager();
	CLapTrackerComponentManager::CreateInstance();
	CKartSpawnPointManager::GetInstance()->Create();
	CPickupComponentManager::Create();
}

void CPlayState::LoadNavigationSpline(const CU::CJsonValue& splineData)
{
	myKartControllerComponentManager->LoadNavigationSpline(splineData);
}

void CPlayState::CreatePlayer(CU::Camera& aCamera, const SParticipant& aParticipant, unsigned int aPlayerCount)
{
	//Create sub sub player object
	CU::CJsonValue playerJson;
	std::string errorString = playerJson.Parse("Json/KartStats.json");
	playerJson = playerJson.at("Karts")[static_cast<short>(aParticipant.mySelectedCharacter)];

	CGameObject* secondPlayerObject = myGameObjectManager->CreateGameObject();
	CModelComponent* playerModel = myModelComponentManager->CreateComponent(playerJson.at("Model").GetString());
	playerModel->SetIsShadowCasting(false);

	CComponent* headLight1 = CLightComponentManager::GetInstance().CreateAndRegisterSpotLightComponent({ 1.f, 1.0f, 0.5f }, 5.f, 5.f, 3.141592f / 16.f);
	CGameObject* headLightObject1 = myGameObjectManager->CreateGameObject();
	headLightObject1->GetLocalTransform().myPosition.Set(-0.45f, 1.f, 1.f);
	headLightObject1->GetLocalTransform().RotateAroundAxis(-3.141592f / 8.f, CU::Axees::X);
	headLightObject1->AddComponent(headLight1);

	secondPlayerObject->AddComponent(headLightObject1);

	CComponent* headLight2 = CLightComponentManager::GetInstance().CreateAndRegisterSpotLightComponent({ 1.f, 1.0f, 0.5f }, 5.f, 5.f, 3.141592f / 16.f);
	CGameObject* headLightObject2 = myGameObjectManager->CreateGameObject();
	headLightObject2->GetLocalTransform().myPosition.Set(0.45f, 1.f, 1.f);
	headLightObject2->GetLocalTransform().RotateAroundAxis(-3.141592f / 8.f, CU::Axees::X);
	headLightObject2->AddComponent(headLight2);

	secondPlayerObject->AddComponent(headLightObject2);

	secondPlayerObject->AddComponent(playerModel);
	secondPlayerObject->AddComponent(new Component::CKartModelComponent(myPhysicsScene));
	//Create sub player object
	CGameObject* intermediary = myGameObjectManager->CreateGameObject();
	intermediary->AddComponent(new Component::CDriftTurner);
	intermediary->AddComponent(secondPlayerObject);
	//Create camera object
	CGameObject* cameraObject = myGameObjectManager->CreateGameObject();
	CCameraComponent* cameraComponent = new CCameraComponent(aPlayerCount);
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(aCamera);
	cameraObject->AddComponent(cameraComponent);
	myCameraComponents.Add(cameraComponent);
	//Create top player object
	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->AddComponent(cameraObject);
	playerObject->AddComponent(intermediary);

	CAudioSourceComponent* audio = CAudioSourceComponentManager::GetInstance().CreateComponent();
	playerObject->AddComponent(audio);

	CU::Matrix44f kartTransformation = CKartSpawnPointManager::GetInstance()->PopSpawnPoint().mySpawnTransformaion;
	playerObject->SetWorldTransformation(kartTransformation);
	

	CRespawnerComponent* respawnComponent = myRespawnComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(respawnComponent);

	if(CLapTrackerComponentManager::GetInstance() != nullptr)
	{
		CLapTrackerComponent* lapTrackerComponent = CLapTrackerComponentManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(lapTrackerComponent);
	}

	CKartControllerComponent* kartComponent = myKartControllerComponentManager->CreateAndRegisterComponent(*playerModel, aParticipant);
	if (myPlayerCount < 2)
	{
		AddXboxController();
		CXboxController* xboxInput = myPlayerControllerManager->CreateXboxController(*kartComponent, static_cast<short>(SParticipant::eInputDevice::eController1));
		CKeyboardController* controls = myPlayerControllerManager->CreateKeyboardController(*kartComponent);
	}
	else
	{
		if (aParticipant.myInputDevice == SParticipant::eInputDevice::eKeyboard)
		{
			CKeyboardController* controls = myPlayerControllerManager->CreateKeyboardController(*kartComponent);
		}
		else
		{
			CXboxController* xboxInput = myPlayerControllerManager->CreateXboxController(*kartComponent, static_cast<short>(aParticipant.myInputDevice));
		}
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
	/*SBoxColliderData box;
	box.myHalfExtent = CU::Vector3f(1.f, 1.0f, 1.f);
	box.center.y = 1.05f;
	box.myLayer = Physics::eKart;
	box.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);*/

	const CU::Vector3f center = { 0.f,0.0f, 0.f };

	SBoxColliderData triggerbox;
	triggerbox.myHalfExtent = CU::Vector3f(.5f, .5f, 0.5f);
	triggerbox.center = center;
	triggerbox.myLayer = Physics::eKart;
	triggerbox.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);
	

	//CColliderComponent* playerColliderComponent = myColliderComponentManager->CreateComponent(&box, playerObject->GetId());
	triggerbox.IsTrigger = false;
	CColliderComponent* playerTriggerColliderComponent = myColliderComponentManager->CreateComponent(&triggerbox, playerObject->GetId());

	
	SRigidBodyData rigidbodah;
	rigidbodah.isKinematic = true;
	rigidbodah.useGravity = false;
	rigidbodah.myLayer = Physics::eKart;

	rigidbodah.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);
	CColliderComponent* rigidComponent = myColliderComponentManager->CreateComponent(&rigidbodah, playerObject->GetId());
//	colliderObject->SetWorldPosition({ offset.x, offset.y + 0.1f, offset.z });
	//playerObject->AddComponent(playerColliderComponent);
	playerObject->AddComponent(playerTriggerColliderComponent);
	playerObject->AddComponent(rigidComponent);
	playerObject->AddComponent(new CCharacterInfoComponent(aParticipant.mySelectedCharacter, false));

	


	CPollingStation::GetInstance()->AddPlayer(playerObject);

	//playerObject->Move(CU::Vector3f(0, 10, 0));

	myKartObjects.Add(playerObject);
}

void CPlayState::CreateAI()
{

	CGameObject* secondPlayerObject = myGameObjectManager->CreateGameObject();
	CModelComponent* playerModel = myModelComponentManager->CreateComponent("Models/Animations/M_Kart_01.fbx");
	playerModel->SetIsShadowCasting(false);
	secondPlayerObject->AddComponent(playerModel);
	secondPlayerObject->AddComponent(new Component::CKartModelComponent(myPhysicsScene));

	CGameObject* intermediary = myGameObjectManager->CreateGameObject();
	intermediary->AddComponent(new Component::CDriftTurner);
	intermediary->AddComponent(secondPlayerObject);

	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->AddComponent(intermediary);

	CU::Matrix44f kartTransformation = CKartSpawnPointManager::GetInstance()->PopSpawnPoint().mySpawnTransformaion;
	playerObject->SetWorldTransformation(kartTransformation);

	CRespawnerComponent* respawnComponent = myRespawnComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(respawnComponent);

	if (CLapTrackerComponentManager::GetInstance() != nullptr)
	{
		CLapTrackerComponent* lapTrackerComponent = CLapTrackerComponentManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(lapTrackerComponent);
	}

	CKartControllerComponent* kartComponent = myKartControllerComponentManager->CreateAndRegisterComponent(*playerModel);

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
	box.myHalfExtent = CU::Vector3f(.5f, .5f, .5f);
	box.myLayer = Physics::eKart;
	box.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);

	SBoxColliderData triggerbox;
	triggerbox.myHalfExtent = CU::Vector3f(0.5f, 0.5f, 0.5f);
	triggerbox.myLayer = Physics::eKart;
	triggerbox.myCollideAgainst = Physics::GetCollideAgainst(Physics::eKart);
	triggerbox.IsTrigger = false;

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
	playerObject->AddComponent(new CCharacterInfoComponent(SParticipant::eCharacter::eVanBrat, true));

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
		myIsCountingDown = true;

		Audio::CAudioInterface::GetInstance()->PostEvent("PlayStartCountDown");
		Audio::CAudioInterface::GetInstance()->PostEvent("PlayEngineIdle");
		for (int i = 0; i < myKartObjects.Size(); i++)
		{
			SComponentMessageData data; data.myString = "PlayEngineStart";
			myKartObjects[i]->NotifyOnlyComponents(eComponentMessageType::ePlaySound, data);
		}

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
					POSTMASTER.Broadcast(new CRaceStartedMessage());
					POSTMASTER.GetThreadOffice().HandleMessages();
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

		myIsCountingDown = false;
		myCountdownShouldRender = false;


		CU::CJsonValue levelsFile;
		std::string errorString = levelsFile.Parse("Json/LevelList.json");
		if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

		CU::CJsonValue levelsArray = levelsFile.at("levels");

		const char* song = levelsArray.at(myLevelIndex).GetString().c_str();

		Audio::CAudioInterface::GetInstance()->PostEvent(song);

	};

	work.SetFinishedCallback(callback);
	CU::ThreadPool::GetInstance()->AddWork(work);
}

void CPlayState::RenderCountdown()
{
	SCreateOrClearGuiElement* createOrClear = new SCreateOrClearGuiElement(L"countdown", *myCountdownElement, WINDOW_SIZE);
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

void CPlayState::LoadPlacementLineGUI()
{
	CU::CJsonValue jsonDoc;
	if (myPlayers.Size() == 1)
	{
		jsonDoc.Parse("Json/HUD/HUD1Player.json");
	}
	else if (myPlayers.Size() == 2)
	{
		jsonDoc.Parse("Json/HUD/HUD2Player.json");
	}
	else if (myPlayers.Size() == 3)
	{
		jsonDoc.Parse("Json/HUD/HUD3Player.json");
	}
	else if (myPlayers.Size() == 4)
	{
		jsonDoc.Parse("Json/HUD/HUD4Player.json");
	}

	CU::CJsonValue jsonPlacementLine = jsonDoc.at("placementLine");
	for(unsigned int i = 0; i < myKartObjects.Size(); i++)
	{
		SHUDElement* hudElement = new SHUDElement();

		hudElement->myGUIElement.myOrigin = { 0.f,0.f }; // { 0.5f, 0.5f };
		hudElement->myGUIElement.myAnchor[(char)eAnchors::eTop] = true;
		hudElement->myGUIElement.myAnchor[(char)eAnchors::eLeft] = true;

		hudElement->myGUIElement.myScreenRect = CU::Vector4f(jsonPlacementLine.at("position").GetVector2f());

		const CU::CJsonValue sizeObject = jsonPlacementLine.at("size");
		hudElement->myPixelSize.x = sizeObject.at("pixelWidth").GetUInt();
		hudElement->myPixelSize.y = sizeObject.at("pixelHeight").GetUInt();

		float rectWidth = sizeObject.at("screenSpaceWidth").GetFloat();
		float rectHeight = sizeObject.at("screenSpaceHeight").GetFloat();
		myPlacementLineScreenSpaceWidth = rectWidth;

		float topLeftX = hudElement->myGUIElement.myScreenRect.x;
		float topLeftY = hudElement->myGUIElement.myScreenRect.y;

		hudElement->myGUIElement.myScreenRect.z = rectWidth + topLeftX;
		hudElement->myGUIElement.myScreenRect.w = rectHeight + topLeftY;
		hudElement->mySprite = new CSpriteInstance("Sprites/GUI/Scoreboard/characterPortraitYoshi.dds", { 1.0f, 1.0f });
		myPlacementLinesGUIElement.Add(hudElement);
	}
}

void CPlayState::RenderPlacementLine()
{
	for(unsigned int i = 0; i < myPlacementLinesGUIElement.Size(); i++)
	{
		/*SComponentQuestionData lapTraversedPercentageQuestionData;
		if (myKartObjects[i]->AskComponents(eComponentQuestionType::eGetLapTraversedPercentage, lapTraversedPercentageQuestionData) == true)
		{
			float lapTraversedPlacement = lapTraversedPercentageQuestionData.myFloat;
			myPlacementLinesGUIElement[i]->myGUIElement.myScreenRect.x = lapTraversedPlacement;
			myPlacementLinesGUIElement[i]->myGUIElement.myScreenRect.z = myPlacementLineScreenSpaceWidth + lapTraversedPlacement;
		}

		SCreateOrClearGuiElement* createOrClear = new SCreateOrClearGuiElement(L"placementLine" + i, myPlacementLinesGUIElement[i]->myGUIElement, CU::Vector2ui(WINDOW_SIZE.x, WINDOW_SIZE.y));
		RENDERER.AddRenderMessage(createOrClear);

		SChangeStatesMessage* const changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eAlphaBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		SRenderToGUI* const guiChangeState = new SRenderToGUI(L"placementLine" + i, changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		myPlacementLinesGUIElement[i]->mySprite->RenderToGUI(L"placementLine" + i);*/
	}
}
