#pragma once
#include "../StateStack/State.h"
#include "../PostMaster/Subscriber.h"
#include <atomic>
#include "../ThreadedPostmaster/Subscriber.h"

namespace CU
{
	class Time;
	class Camera;
}

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

class CPlayState : public State , public Postmaster::ISubscriber
{
public:
	CPlayState(StateStack& aStateStack, const int aLevelIndex);
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

	inline bool IsLoaded() const;

	eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage) override;
	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;
	inline void SetCameraComponent(CCameraComponent* aCameraComponent);
private:
	void SpawnPlayer(CU::Camera& aCamera);
private:
	Physics::CPhysicsScene* myPhysicsScene;
	Physics::CPhysics* myPhysics;



	CColliderComponentManager* myColliderComponentManager;

	CGameObjectManager* myGameObjectManager;
	CScene* myScene;

	CModelComponentManager* myModelComponentManager;

	CScriptComponentManager* myScriptComponentManager;

	class CCameraComponent* myCameraComponent;
	bool myIsInfocus;

	int myLevelIndex;
	std::atomic_bool myIsLoaded;

	bool myPressedAnyKey;
	int mode;
	//Super temp ta bort när guit börjar ta form
	//CTextInstance* myPlayerHealthText;
	//CTextInstance* myPlayerArmorText;

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

void CPlayState::SetCameraComponent(CCameraComponent* aCameraComponent)
{
	myCameraComponent = aCameraComponent;
}