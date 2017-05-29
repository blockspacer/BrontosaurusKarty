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

	void Init(Physics::CPhysicsScene* aPhysicsScene, CKartControllerComponentManager* aKartManager);

	void Update(const float aDeltaTime);

	void DoPhysics(const float aDeltaTime);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::Vector3f myVelocity;
	Physics::CPhysicsScene* myPhysicsScene;
	CKartControllerComponentManager* myKartManager;

	int myCurrentSplineIndex;
	float Speed;

};

