#pragma once

class CItemWeaponBehaviourComponent;

namespace Physics
{
	class CPhysicsScene;
}

class CItemWeaponBehaviourComponentManager
{
public:
	CItemWeaponBehaviourComponentManager();
	~CItemWeaponBehaviourComponentManager();
	
	void Init(Physics::CPhysicsScene* aPhysicsScene);

	CItemWeaponBehaviourComponent* CreateAndRegisterComponent();

	void Update(const float aDeltatime);

private:
	CU::GrowingArray<CItemWeaponBehaviourComponent*> myComponents;

	Physics::CPhysicsScene* myPhysicsScene;
};

