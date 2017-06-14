#pragma once
#include "Component.h"

namespace Physics
{
	class CPhysicsScene;
}

class CKartControllerComponentManager;

class CRedShellBehaviourComponent: public CComponent
{
public:
	CRedShellBehaviourComponent();
	~CRedShellBehaviourComponent();

	void Init(Physics::CPhysicsScene* aPhysicsScene, CKartControllerComponentManager* aKartManager, CU::GrowingArray<CGameObject*>& aListOfKarts);

	void Update(const float aDeltaTime);

	void DoPhysics(const float aDeltaTime);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::Vector3f myVelocity;
	CGameObject* myCurrentUser;
	CGameObject* myLastTarget;

	Physics::CPhysicsScene* myPhysicsScene;
	CKartControllerComponentManager* myKartManager;
	CU::GrowingArray<CGameObject*>* myKartObjects;



	int myCurrentSplineIndex;
	int myUserPlacement;
	float mySpeed;

	bool myPlayingWarning;
	bool myIsActive;
};

