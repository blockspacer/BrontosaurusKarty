#pragma once
#include "ServerMain.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../CommonUtilities/TimerManager.h"
#include "../Physics/Foundation.h"
#include "../Physics/Physics.h"
#include "../Game/BackgroundLoadingManager.h"

class CGameObjectManager;

class CGameServer
{
public:
	CGameServer();
	~CGameServer();

	void Init();
	void Start();

	CGameObjectManager& GetGameObjectManager();

	void Load(const int aLevelIndex);
	void ReInit();

	void CreateManagersAndFactories();
	void DestroyManagersAndFactories();
	bool Update(CU::Time aDeltaTime);

	bool IsLoaded() const;
	std::thread::id& GetThreadID();
	inline CColliderComponentManager* GetColliderComponentManager();
	inline const short GetCurrentLevelIndex();
private:
	CGameObjectManager* myGameObjectManager;

	CU::TimerManager myTimerManager;
	CU::TimerHandle myMainTimer;

	CU::ThreadPool* myThreadPool;

	std::thread::id myThreadID;


	short myLevelIndex;

	bool myIsRunning;
	bool myInGame;
	std::atomic_bool myIsLoaded;

	float myTime;
	Physics::CPhysics* myPhysics;
	Physics::CPhysicsScene* myPhysicsScene;
	CColliderComponentManager* myColliderComponentManager;
};

inline CColliderComponentManager* CGameServer::GetColliderComponentManager()
{
	return myColliderComponentManager;
}

inline const short CGameServer::GetCurrentLevelIndex()
{
	return myLevelIndex;
}