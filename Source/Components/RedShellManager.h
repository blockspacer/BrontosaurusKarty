#pragma once

namespace Physics
{
	class CPhysicsScene;
}
class CKartControllerComponentManager;
class CRedShellBehaviourComponent;

class CRedShellManager
{
public:
	CRedShellManager();
	~CRedShellManager();

	void Init(Physics::CPhysicsScene * aPhysicsScene, CKartControllerComponentManager* aKartManager, CU::GrowingArray<CGameObject*>& aListOfKarts);

	void Update(const float aDeltatime);

	CRedShellBehaviourComponent* CreateAndRegisterComponent();

private:

	CU::GrowingArray<CRedShellBehaviourComponent*> myComponents;

	Physics::CPhysicsScene* myPhysicsScene;

	CKartControllerComponentManager* myKartManager;

	CU::GrowingArray<CGameObject*>* myListOfKarts;
};

