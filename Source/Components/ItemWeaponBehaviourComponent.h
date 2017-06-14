#pragma once
#include "Component.h"

namespace Physics
{
	class CPhysicsScene;
}



class CItemWeaponBehaviourComponent: public CComponent
{
public:
	CItemWeaponBehaviourComponent();
	~CItemWeaponBehaviourComponent();

	void Init(Physics::CPhysicsScene* aPhysicsScene);

	void SetToNoSpeed();

	void Update(const float aDeltaTime);

	void DoPhysics(const float aDeltaTime);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:

	Physics::CPhysicsScene* myPhysicsScene;

	CU::Vector3f myVelocity;

	float mySpeed;

	float myGrip;
	float myWeight;
	bool myIsOnGround;

	bool myIsActive;

};

