#pragma once
#include "../Game/State.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include "SParticipant.h"
#include "NavigationSpline.h"

namespace CU
{
	class TimerManager;
	class Time;
	class Camera;
}

typedef unsigned int TimerHandle;

namespace Physics
{
	class CPhysicsScene;
	class CPhysics;
}

class CShowTitleComponent;

class CGameObjectManager;
class CScene;
class CModelComponentManager;
class CColliderComponentManager;
class CScriptComponentManager;
class CTextInstance;
class CSpriteInstance;
class CGameObject;
class CCameraComponent;
class CKartComponentManager;
class CKartControllerComponentManager;
class CPlayerControllerManager;
class CBoostPadComponentManager;
class CItemFactory;
class CRespawnComponentManager;

class CPlayState : public State , public Postmaster::ISubscriber
{
public:
	CPlayState(StateStack& aStateStack, const int aLevelIndex);
	CPlayState(StateStack& aStateStack, const int aLevelIndex, const CU::GrowingArray<SParticipant> aPlayers);
	~CPlayState();

	void Load();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;


	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	void CreateManagersAndFactories();



	CGameObjectManager* GetGameObjectManager();
	inline CColliderComponentManager* GetColliderComponentManager();
	inline CScriptComponentManager* GetScriptComponentManager();
	inline CItemFactory* GetItemFactory();

	inline bool IsLoaded() const;

	eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage) override;
	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;
	void SetCameraComponent(CCameraComponent* aCameraComponent);
	inline CBoostPadComponentManager* GetBoostPadComponentManager();

	void LoadNavigationSpline(const CU::CJsonValue &splineData);
private:
	void CreatePlayer(CU::Camera& aCamera, const SParticipant::eInputDevice aIntputDevice);
	void InitiateRace();

private:
	Physics::CPhysicsScene* myPhysicsScene;
	Physics::CPhysics* myPhysics;

	CGameObjectManager* myGameObjectManager;
	CScene* myScene;

	CModelComponentManager* myModelComponentManager;
	CColliderComponentManager* myColliderComponentManager;
	CScriptComponentManager* myScriptComponentManager;
	CKartComponentManager* myKartComponentManager;
	CBoostPadComponentManager* myBoostPadComponentManager;

	CKartControllerComponentManager* myKartControllerComponentManager;
	CPlayerControllerManager* myPlayerControllerManager;
	CItemFactory* myItemFactory;
	CRespawnComponentManager* myRespawnComponentManager;

	CU::GrowingArray<CCameraComponent*> myCameraComponents;
	CU::GrowingArray<SParticipant> myPlayers;

	//CU::TimerManager* myTimerManager;
	TimerHandle myCountdownTimerHandle;

	int myPlayerCount;
	int myLevelIndex;
	std::atomic_bool myIsLoaded;

};

inline bool CPlayState::IsLoaded() const
{
	return myIsLoaded;
}

inline CColliderComponentManager* CPlayState::GetColliderComponentManager()
{
	return myColliderComponentManager;
}

inline CScriptComponentManager* CPlayState::GetScriptComponentManager()
{
	return myScriptComponentManager;
}

inline CBoostPadComponentManager* CPlayState::GetBoostPadComponentManager()
{
	return myBoostPadComponentManager;
}

inline CItemFactory* CPlayState::GetItemFactory()
{
	return myItemFactory;
}
