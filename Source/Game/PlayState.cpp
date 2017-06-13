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
#include "TimeTrackerComponentManager.h"
#include "SailInCirclesManager.h"

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
#include "../CommonUtilities/XInputWrapper.h"

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
#include "BroadcastINputListener.h"
#include "InputManager.h"
#include "TimeTrackerComponent.h"
#include "DecalComponent.h"
#include "MenuState.h"
#include "C3DSpriteComponent.h"
#include "CameraTilter.h"
#include "SplitScreenSelection.h"

CPlayState::CPlayState(StateStack & aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myPhysicsScene(nullptr)
	, myPhysics(nullptr)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myModelComponentManager(nullptr)
	, myColliderComponentManager(nullptr)
	, myScriptComponentManager(nullptr)
	, myKartComponentManager(nullptr)
	, myBoostPadComponentManager(nullptr)
	, myKartControllerComponentManager(nullptr)
	, myPlayerControllerManager(nullptr)
	, myItemFactory(nullptr)
	, myItemBehaviourManager(nullptr)
	, myRedShellManager(nullptr)
	, myBlueShellManager(nullptr)
	, myRespawnComponentManager(nullptr)
	, myExplosionManager(nullptr)
	, myTimeTrackerComponentManager(nullptr)
	, myCameraComponents(4)
	, myGlobalHUD(nullptr)
	, myPlayerCount(1)
	, myLevelIndex(aLevelIndex)
	, myIsLoaded(false)
	//, myCountdownShouldRender(false)
	, myIsCountingDown(true)
{
	myPlayers.Init(1);
	myPlayerCount = 1;
	myPlayers.Add(SParticipant());
	myPlayers[0].myInputDevice = SParticipant::eInputDevice::eKeyboard;
	//myPlacementLinesGUIElement.Init(8);
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
	, myKartComponentManager(nullptr)
	, myBoostPadComponentManager(nullptr)
	, myKartControllerComponentManager(nullptr)
	, myPlayerControllerManager(nullptr)
	, myItemFactory(nullptr)
	, myItemBehaviourManager(nullptr)
	, myRedShellManager(nullptr)
	, myBlueShellManager(nullptr)
	, myRespawnComponentManager(nullptr)
	, myExplosionManager(nullptr)
	, myTimeTrackerComponentManager(nullptr)
	, myCameraComponents(4)
	, myGlobalHUD(nullptr)
	, myPlayerCount(1)
	, myLevelIndex(aLevelIndex)
	, myIsLoaded(false)
	, myIsCountingDown(false)

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

	//myPlacementLinesGUIElement.Init(8);
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
	SAFE_DELETE(myTimeTrackerComponentManager);
	SAFE_DELETE(myRedShellManager);
	SAFE_DELETE(myBlueShellManager);
	SAFE_DELETE(myExplosionManager);

	SAFE_DELETE(myGlobalHUD);
	CLapTrackerComponentManager::DestoyInstance();

	CKartSpawnPointManager::GetInstance()->Destroy();
	CPickupComponentManager::Destroy();

	myLocalHUDs.DeleteAll();

	CPollingStation::Destroy();
	CSailInCirclesManager::Destroy();
}

// Runs on its own thread.
void CPlayState::Load()
{
	CPollingStation::Create();
	CU::CStopWatch loadPlaystateTimer;
	loadPlaystateTimer.Start();

	//myCountdownSprite = new CSpriteInstance("Sprites/GUI/countdown.dds");
	//myCountdownSprite->SetPivot({ 0.5f,0.5f });
	//myCountdownSprite->SetSize({ 0.25f,0.25f });
	//myCountdownSprite->SetRect({ 0.f,0.75f,1.f,1.f });
	//myCountdownSprite->SetPosition({ 0.5f,0.5f });
	//
	//myCountdownElement = new SGUIElement();
	//myCountdownElement->myAnchor.Set((unsigned int)eAnchors::eTop);
	//myCountdownElement->myAnchor.Set((unsigned int)eAnchors::eLeft);
	//myCountdownElement->myScreenRect.Set(0.f, 0.f, 1.f, 1.f);
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

	myLevelsCount = levelsArray.Size();
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
	int currentPlayer = 0;
	for (int i = 0; i < myPlayerCount; ++i)
	{
		CreatePlayer(myScene->GetPlayerCamera(i).GetCamera(), myPlayers[i], myPlayerCount, currentPlayer++);
	}

	for (int i = 0; i < 8 - myPlayerCount; ++i)
	{
		CreateAI();
	}

	CPollingStation::GetInstance()->BindKartList(&myKartObjects);

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

	myGlobalHUD = new CGlobalHUD(myLevelIndex);
	myGlobalHUD->LoadHUD();


	//myCountdownSprite->Render();

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
		POSTMASTER.Subscribe(myLocalHUDs[i], eMessageType::eRedShellWarning);
		POSTMASTER.Subscribe(myLocalHUDs[i], eMessageType::eCharPressed);
	}

	POSTMASTER.Subscribe(myGlobalHUD, eMessageType::eCharPressed);
	POSTMASTER.Subscribe(myGlobalHUD, eMessageType::eRaceOver);
	POSTMASTER.Subscribe(myGlobalHUD, eMessageType::eControllerInput);

	POSTMASTER.Subscribe(myPlayerControllerManager, eMessageType::ePlayerFinished);
	POSTMASTER.Subscribe(myPlayerControllerManager, eMessageType::eRaceStarted);
	POSTMASTER.Subscribe(this, eMessageType::eControllerInput);

	POSTMASTER.Subscribe(myTimeTrackerComponentManager, eMessageType::eRaceStarted);
	POSTMASTER.Subscribe(myKartControllerComponentManager, eMessageType::eRaceStarted);


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
	if (myTimeTrackerComponentManager != nullptr)
	{
		myTimeTrackerComponentManager->Update(aDeltaTime.GetSeconds());
	}

	myRedShellManager->Update(aDeltaTime.GetSeconds());
	myBlueShellManager->Update(aDeltaTime.GetSeconds());
	myExplosionManager->Update(aDeltaTime.GetSeconds());

	CSailInCirclesManager::GetInstance().Update(aDeltaTime.GetSeconds());

	CPickupComponentManager::GetInstance()->Update(aDeltaTime.GetSeconds());
	return myStatus;
}

void CPlayState::Render()
{
	myScene->RenderSplitScreen(myPlayerCount);

	//if (myCountdownShouldRender)
	//	RenderCountdown();

	for (int i = 0; i < myPlayerCount; ++i)
	{
		myLocalHUDs[i]->Render();
	}

	myGlobalHUD->Render();

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
	myGlobalHUD->StartCountDown();
	//InitiateRace();

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
		myStateStack.PushState(new CMenuState(myStateStack, "Json/Menu/PauseMenu.json", this));
		return CU::eInputReturn::eKeepSecret;
	}
	switch(aInputMessage.myType)
	{
	case CU::eInputType::eKeyboardPressed:
	case CU::eInputType::eGamePadButtonPressed:
	if(aInputMessage.myGamePad == CU::GAMEPAD::B || aInputMessage.myKey == CU::eKeys::RCONTROL)
	{
		Postmaster::Message::InputEventData eventData;
		eventData.eventType = Postmaster::Message::EventType::ButtonChanged;
		eventData.data.boolValue = true;
		eventData.buttonIndex = Postmaster::Message::ButtonIndex::B;
		PostPostmasterEvent(aInputMessage.myGamepadIndex, eventData);
	}
	if (aInputMessage.myGamePad == CU::GAMEPAD::A || aInputMessage.myKey == CU::eKeys::SPACE)
	{
		Postmaster::Message::InputEventData eventData;
		eventData.eventType = Postmaster::Message::EventType::ButtonChanged;
		eventData.data.boolValue = true;
		eventData.buttonIndex = Postmaster::Message::ButtonIndex::A;
		PostPostmasterEvent(aInputMessage.myGamepadIndex, eventData);
	}
	if (aInputMessage.myGamePad == CU::GAMEPAD::X || aInputMessage.myKey == CU::eKeys::RETURN)
	{
		Postmaster::Message::InputEventData eventData;
		eventData.eventType = Postmaster::Message::EventType::ButtonChanged;
		eventData.data.boolValue = true;
		eventData.buttonIndex = Postmaster::Message::ButtonIndex::X;
		PostPostmasterEvent(aInputMessage.myGamepadIndex, eventData);
		
	}
	break;

	case CU::eInputType::eGamePadButtonReleased: 
		
		break;
	
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
	myBlueShellManager = new CBlueShellComponentManager(myKartObjects,myGameObjectManager,myExplosionManager,myColliderComponentManager);
	myItemFactory = new CItemFactory();
	myItemFactory->Init(*myGameObjectManager, *myItemBehaviourManager, myPhysicsScene, *myColliderComponentManager,*myRedShellManager,*myBlueShellManager,myScene);
	myRespawnComponentManager = new CRespawnComponentManager();
	myTimeTrackerComponentManager = new CTimeTrackerComponentManager();
	CLapTrackerComponentManager::CreateInstance();
	CKartSpawnPointManager::GetInstance()->Create();
	CPickupComponentManager::Create();

	CSailInCirclesManager::CreateInstance();
}

void CPlayState::LoadNavigationSpline(const CU::CJsonValue& splineData)
{
	myKartControllerComponentManager->LoadNavigationSpline(splineData);
}

void CPlayState::PostPostmasterEvent(short aGamepadIndex,const Postmaster::Message::InputEventData& aEventData)
{
	POSTMASTER.Broadcast(new Postmaster::Message::CControllerInputMessage(aGamepadIndex,aEventData));
}

const CU::Vector4f CPlayState::GetPlayerColor(const int aInputDevice)
{
	CU::Vector4f color;
	switch (aInputDevice)
	{
	case 0:

		color = YELLOW;
		
		break;
	case 1:

		color = GREEN;
		
		break;
	case 2:

		color = PINK;
		
		break;
	case 3:

		color = BLUE;
		
		break;
	default:
		color = DEFAULT;
		// Also make mark a bit smaller.
		break;
	}

	return color;
}

void CPlayState::CreatePlayer(CU::Camera& aCamera, const SParticipant& aParticipant, unsigned int aPlayerCount, int aCurrentPlayer)
{
	//Create sub sub player object
	CU::CJsonValue playerJson;
	std::string errorString = playerJson.Parse("Json/KartStats.json");
	playerJson = playerJson.at("Karts")[static_cast<short>(aParticipant.mySelectedCharacter)];

	CGameObject* secondPlayerObject = myGameObjectManager->CreateGameObject();
	CModelComponent* playerModel = myModelComponentManager->CreateComponent(playerJson.at("Model").GetString());
	playerModel->SetIsShadowCasting(false);



	// Lights
	//


	CComponent* headLight1 = CLightComponentManager::GetInstance().CreateAndRegisterSpotLightComponent({ 1.f, 1.0f, 0.5f }, 2.f, 5.f, 3.141592f / 16.f);
	CGameObject* headLightObject1 = myGameObjectManager->CreateGameObject();
	headLightObject1->GetLocalTransform().myPosition.Set(-0.45f, 1.f, 1.f);
	headLightObject1->GetLocalTransform().RotateAroundAxis(-3.141592f / 8.f, CU::Axees::X);
	headLightObject1->AddComponent(headLight1);

	secondPlayerObject->AddComponent(headLightObject1);

	CComponent* headLight2 = CLightComponentManager::GetInstance().CreateAndRegisterSpotLightComponent({ 1.f, 1.0f, 0.5f }, 2.f, 5.f, 3.141592f / 16.f);
	CGameObject* headLightObject2 = myGameObjectManager->CreateGameObject();
	headLightObject2->GetLocalTransform().myPosition.Set(0.45f, 1.f, 1.f);
	headLightObject2->GetLocalTransform().RotateAroundAxis(-3.141592f / 8.f, CU::Axees::X);
	headLightObject2->AddComponent(headLight2);

	secondPlayerObject->AddComponent(headLightObject2);


	//
	// decal
	CDecalComponent* decal = new CDecalComponent(*myScene);
	CComponentManager::GetInstance().RegisterComponent(decal);
	decal->SetDecalIndex(0);

	CGameObject* decalHolder = myGameObjectManager->CreateGameObject();
	decalHolder->GetLocalTransform().myPosition.Set(0.0f, -0.88f, 0.4f);
	decalHolder->GetLocalTransform().SetScale({ 2.0f, 2.f, 2.f });
	decalHolder->AddComponent(decal);
	secondPlayerObject->AddComponent(decalHolder);
	//

	secondPlayerObject->AddComponent(playerModel);
	CComponent* kartModelComponent = new Component::CKartModelComponent(myPhysicsScene);
	CComponentManager::GetInstance().RegisterComponent(kartModelComponent);
	secondPlayerObject->AddComponent(kartModelComponent);
	//Create sub player object
	CGameObject* intermediary = myGameObjectManager->CreateGameObject();
	CComponent* driftTurnerComponent = new Component::CDriftTurner();
	CComponentManager::GetInstance().RegisterComponent(driftTurnerComponent);
	intermediary->AddComponent(driftTurnerComponent);
	intermediary->AddComponent(secondPlayerObject);
	//Create camera object
	CGameObject* cameraObject = myGameObjectManager->CreateGameObject();
	CCameraComponent* cameraComponent = new CCameraComponent(aPlayerCount);
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(aCamera);
	cameraObject->AddComponent(cameraComponent);
	myCameraComponents.Add(cameraComponent);

	CGameObject* cameraObjectSecond = myGameObjectManager->CreateGameObject();
	CCameraTilter* cameraTilter = new CCameraTilter(myPhysicsScene);
	cameraObjectSecond->AddComponent(cameraTilter);
	cameraObjectSecond->AddComponent(cameraObject);

	//Create player number object
	CGameObject* playerNumber = myGameObjectManager->CreateGameObject();
	C3DSpriteComponent* numberModel = new C3DSpriteComponent(*myScene, "Sprites/GUI/playerMarker.dds", CU::Vector2f::One * 5.f, CU::Vector2f(0.5f,0.5f),
		CU::Vector4f(0.f,0.f,1.f,1.f), GetPlayerColor(aCurrentPlayer), aCurrentPlayer);
	CComponentManager::GetInstance().RegisterComponent(numberModel);
	playerNumber->AddComponent(numberModel);
	playerNumber->GetLocalTransform().SetPosition({ 0.f,1.5f,0.f });

	//Create top player object
	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->AddComponent(cameraObjectSecond);
	playerObject->AddComponent(intermediary);
	playerObject->AddComponent(playerNumber);

	CAudioSourceComponent* audio = CAudioSourceComponentManager::GetInstance().CreateComponent();
	playerObject->AddComponent(audio);

	CU::Matrix44f kartTransformation = CKartSpawnPointManager::GetInstance()->PopSpawnPoint().mySpawnTransformaion;
	playerObject->SetWorldTransformation(kartTransformation);
	

	CRespawnerComponent* respawnComponent = myRespawnComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(respawnComponent);

	CTimeTrackerComponent* timeTrackerComponent = myTimeTrackerComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(timeTrackerComponent);


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
	CComponent* characterInfoComponent = new CCharacterInfoComponent(aParticipant.mySelectedCharacter, false);
	CComponentManager::GetInstance().RegisterComponent(characterInfoComponent);
	playerObject->AddComponent(characterInfoComponent);

	


	CPollingStation::GetInstance()->AddPlayer(playerObject);

	//playerObject->Move(CU::Vector3f(0, 10, 0));

	myKartObjects.Add(playerObject);
}

void CPlayState::CreateAI()
{
	static short i = 0;
	SParticipant::eCharacter character;
	character = static_cast<SParticipant::eCharacter>(i);
	++i;
	if (i >= static_cast<short>(SParticipant::eCharacter::eLength))
	{
		i = rand() % (static_cast<short>(SParticipant::eCharacter::eLength) - 1);
	}
	CU::CJsonValue playerJson;
	std::string errorString = playerJson.Parse("Json/KartStats.json");
	playerJson = playerJson.at("Karts")[static_cast<short>(character)];

	CGameObject* secondPlayerObject = myGameObjectManager->CreateGameObject();
	CModelComponent* playerModel = myModelComponentManager->CreateComponent(playerJson.at("Model").GetString());
	playerModel->SetIsShadowCasting(false);
	secondPlayerObject->AddComponent(playerModel);
	CComponent* kartModelComponent = new Component::CKartModelComponent(myPhysicsScene);
	CComponentManager::GetInstance().RegisterComponent(kartModelComponent);
	secondPlayerObject->AddComponent(kartModelComponent);

	CGameObject* intermediary = myGameObjectManager->CreateGameObject();
	CComponent* driftTurnerComponent = new Component::CDriftTurner();
	CComponentManager::GetInstance().RegisterComponent(driftTurnerComponent);
	intermediary->AddComponent(driftTurnerComponent);
	intermediary->AddComponent(secondPlayerObject);

	//Create player number object
	CGameObject* playerNumber = myGameObjectManager->CreateGameObject();
	C3DSpriteComponent* numberModel = new C3DSpriteComponent(*myScene, "Sprites/GUI/playerMarker.dds", CU::Vector2f::One, CU::Vector2f(0.5f, 0.5f),
		CU::Vector4f(0.f, 0.f, 1.f, 1.f), DEFAULT);
	CComponentManager::GetInstance().RegisterComponent(numberModel);
	playerNumber->AddComponent(numberModel);
	playerNumber->GetLocalTransform().SetPosition({ 0.f,2.f,0.f });


	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->AddComponent(intermediary);
	playerObject->AddComponent(playerNumber);

	//
	// decal
	CDecalComponent* decal = new CDecalComponent(*myScene);
	CComponentManager::GetInstance().RegisterComponent(decal);
	decal->SetDecalIndex(0);

	CGameObject* decalHolder = myGameObjectManager->CreateGameObject();
	decalHolder->GetLocalTransform().myPosition.Set(0.0f, -0.88f, 0.4f);
	decalHolder->GetLocalTransform().SetScale({ 2.0f, 2.f, 2.f });
	decalHolder->AddComponent(decal);
	secondPlayerObject->AddComponent(decalHolder);
	//

	CU::Matrix44f kartTransformation = CKartSpawnPointManager::GetInstance()->PopSpawnPoint().mySpawnTransformaion;
	playerObject->SetWorldTransformation(kartTransformation);

	CRespawnerComponent* respawnComponent = myRespawnComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(respawnComponent);

	CTimeTrackerComponent* timeTrackerComponent = myTimeTrackerComponentManager->CreateAndRegisterComponent();
	playerObject->AddComponent(timeTrackerComponent);

	if (CLapTrackerComponentManager::GetInstance() != nullptr)
	{
		CLapTrackerComponent* lapTrackerComponent = CLapTrackerComponentManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(lapTrackerComponent);
	}
	SParticipant participant;
	participant.mySelectedCharacter = character;
	participant.myInputDevice = static_cast<SParticipant::eInputDevice>(-1);

	CKartControllerComponent* kartComponent = myKartControllerComponentManager->CreateAndRegisterComponent(*playerModel,participant);

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

	CComponent* characterInfoComnponent = new CCharacterInfoComponent(SParticipant::eCharacter::eVanBrat, true);
	CComponentManager::GetInstance().RegisterComponent(characterInfoComnponent);
	playerObject->AddComponent(characterInfoComnponent);

	myKartObjects.Add(playerObject);
}

eMessageReturn CPlayState::DoEvent(const Postmaster::Message::CControllerInputMessage& aControllerInputMessage)
{
	const Postmaster::Message::InputEventData& data = aControllerInputMessage.GetData();

	if (myGlobalHUD != nullptr && myGlobalHUD->GetRaceOver() == true && data.eventType == Postmaster::Message::EventType::ButtonChanged &&
		data.data.boolValue == true && data.buttonIndex == Postmaster::Message::ButtonIndex::B)
	{
		myStateStack.SwapState(new CSplitScreenSelection(myStateStack));
	}
	else if (myGlobalHUD != nullptr && myGlobalHUD->GetRaceOver() == true && data.eventType == Postmaster::Message::EventType::ButtonChanged &&
		data.data.boolValue == true && data.buttonIndex == Postmaster::Message::ButtonIndex::X)
	{
		myStateStack.SwapState(new CLoadState(myStateStack, myLevelIndex, myPlayers));
	}
	else if (myGlobalHUD != nullptr && myGlobalHUD->GetRaceOver() == true && data.eventType == Postmaster::Message::EventType::ButtonChanged &&
		data.data.boolValue == true && data.buttonIndex == Postmaster::Message::ButtonIndex::A)
	{
		myStateStack.SwapState(new CLoadState(myStateStack, (myLevelIndex + 1) % myLevelsCount, myPlayers));
	}

	return eMessageReturn::eContinue;
}
