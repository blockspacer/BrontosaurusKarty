#include "stdafx.h"
#include "GameServer.h"
#include "../KevinLoader/KevinLoader.h"
#include "../KevinLoader/KLoaderError.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PostOffice.h"

//Managers
#include "../LoadManager/ServerLoadManager.h"
#include "../Components/ComponentManager.h"
#include "../Components/GameObjectManager.h"


CGameServer::CGameServer():
	  myGameObjectManager(nullptr)
	, myMainTimer(0)
	, myInGame(false)
	, myIsLoaded(false)
	, myPhysics(nullptr)
	, myPhysicsScene(nullptr)
{
	CU::ThreadPool::Create();
	myThreadPool = CU::ThreadPool::GetInstance();
	myThreadPool->Init();
	myIsRunning = false;
	myTime = 0;

	myThreadID = std::this_thread::get_id();
	myLevelIndex = -1;
}


CGameServer::~CGameServer()
{
	CU::ThreadPool::Destroy();
}

void CGameServer::Init()
{
}

void CGameServer::Start()
{
	
}

CGameObjectManager & CGameServer::GetGameObjectManager()
{
	return *myGameObjectManager;
}

void CGameServer::Load(const int aLevelIndex)
{

	ServerLoadManagerGuard loadManagerGuard(*this);


	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);

	srand(static_cast<unsigned int>(time(nullptr)));

	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();
	CU::CJsonValue levelsFile;

	std::string errorString = levelsFile.Parse("Json/LevelList.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue levelsArray = levelsFile.at("levels");

	myLevelIndex = aLevelIndex;
	std::string levelPath = "Json/Levels/";
	levelPath += levelsArray[myLevelIndex].GetString(); 
	levelPath += "/LevelData.json";

	const KLoader::eError loadError = loader.LoadFile(levelPath);
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}
	myIsLoaded = true;
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
	myGameObjectManager->SendObjectsDoneMessage();
}

void CGameServer::ReInit()
{
	DestroyManagersAndFactories();
	myIsLoaded = false;
}

void CGameServer::CreateManagersAndFactories()
{
	if (Physics::CFoundation::GetInstance() == nullptr)
	{
		Physics::CFoundation::Create();
		myPhysics = Physics::CFoundation::GetInstance()->CreatePhysics();
		myPhysicsScene = myPhysics->CreateScene();
	}

	CComponentManager::CreateInstance();


	myGameObjectManager = new CGameObjectManager();
}

void CGameServer::DestroyManagersAndFactories()
{
	CComponentManager::DestroyInstance();

	SAFE_DELETE(myGameObjectManager);
}

bool CGameServer::Update(CU::Time aDeltaTime)
{


	//DL_PRINT("In Update");
	myTime += aDeltaTime.GetMilliseconds();

	const float updateFrequecy = 0.016f * 1000;
	if(myTime > updateFrequecy)
	{
		//mySpawnerManager->Update(aDeltaTime.GetSeconds() + (updateFrequecy / 1000.0f));
		myTime = 0;
	}
	return true;
}

bool CGameServer::IsLoaded() const
{
	return myIsLoaded;
}

std::thread::id & CGameServer::GetThreadID()
{
	return myThreadID;
}