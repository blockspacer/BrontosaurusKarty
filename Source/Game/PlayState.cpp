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
#include "ItemHolderComponent.h"

#include "KartComponentManager.h"
#include "SpeedHandlerManager.h"
#include "BoostPadComponentManager.h"
#include "ItemFactory.h"
#include "../Components/PickupComponentManager.h"

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
#include "KeyboardController.h"
#include "SpeedHandlerComponent.h"
#include "XboxController.h"

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
	,myItemFactory(nullptr)
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
	SAFE_DELETE(myBoostPadComponentManager);
	SAFE_DELETE(myItemFactory);
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
	myScene->InitPlayerCameras(2);


	CreatePlayer(playerCamera/*myScene->GetPlayerCamera(0)*/.GetCamera());
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
	if (myBoostPadComponentManager != nullptr)
	{
		myBoostPadComponentManager->Update();
	}
	return myStatus;
}

void CPlayState::Render()
{
	myScene->Render();
	//myScene->RenderSplitScreen(2);
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
	myBoostPadComponentManager = new CBoostPadComponentManager();
	myItemFactory = new CItemFactory();
	CKartSpawnPointManager::GetInstance().Create();
	CPickupComponentManager::Create();
}

void CPlayState::CreatePlayer(CU::Camera& aCamera)
{
	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	playerObject->Move(CU::Vector3f::UnitY);
	CModelComponent* playerModel = myModelComponentManager->CreateComponent("Models/Meshes/M_Kart_01.fbx");
	myCameraComponent = new CCameraComponent();
	CComponentManager::GetInstance().RegisterComponent(myCameraComponent);
	myCameraComponent->SetCamera(aCamera);

	CKartControllerComponent* kartComponent = myKartControllerComponentManager->CreateAndRegisterComponent();
	CKeyboardController* controls = myPlayerControllerManager->CreateKeyboardController(*kartComponent);
	if(CSpeedHandlerManager::GetInstance() != nullptr)
	{
		CSpeedHandlerComponent* speedHandlerComponent = CSpeedHandlerManager::GetInstance()->CreateAndRegisterComponent();
		playerObject->AddComponent(speedHandlerComponent);
	}
	CXboxController* xboxInput = myPlayerControllerManager->CreateXboxController(*kartComponent);
	//CXboxControllerComponent* xboxInput = new CXboxControllerComponent();
	//Subscribe(*keyBoardInput);
	//Subscribe(*xboxInput);
	Subscribe(*controls);

	playerObject->AddComponent(playerModel);

	CItemHolderComponent* itemHolder = new CItemHolderComponent(*myItemFactory);
	playerObject->AddComponent(itemHolder);

	playerObject->AddComponent(kartComponent);
	SConcaveMeshColliderData crystalMeshColliderData;
	crystalMeshColliderData.IsTrigger = false;
	crystalMeshColliderData.myPath = "Models/Meshes/M_Kart_01.fbx";
	crystalMeshColliderData.material.aDynamicFriction = 0.5f;
	crystalMeshColliderData.material.aRestitution = 0.5f;
	crystalMeshColliderData.material.aStaticFriction = 0.5f;
	CColliderComponent* playerColliderComponent = myColliderComponentManager->CreateComponent(&crystalMeshColliderData, playerObject->GetId());
	CGameObject* colliderObject = myGameObjectManager->CreateGameObject();
	CU::Vector3f offset = playerObject->GetWorldPosition();
	colliderObject->SetWorldPosition({offset.x,offset.y+0.1f,offset.z});
	colliderObject->AddComponent(playerColliderComponent);

	playerObject->AddComponent(colliderObject);

	playerObject->AddComponent(myCameraComponent);
}

void CPlayState::SetCameraComponent(CCameraComponent* aCameraComponent)
{
	myCameraComponent = aCameraComponent;
}
