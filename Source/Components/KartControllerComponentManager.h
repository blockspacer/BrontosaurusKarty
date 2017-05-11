#pragma once
namespace Physics {
	class CPhysicsScene;
}

class CKartControllerComponent;

class CKartControllerComponentManager
{
public:
	CKartControllerComponentManager();
	~CKartControllerComponentManager();

	CKartControllerComponent* CreateAndRegisterComponent();

	void Update(const float aDeltaTime);
	void Init(Physics::CPhysicsScene* aPhysicsScene);

private:
	CU::GrowingArray<CKartControllerComponent*> myComponents;
	Physics::CPhysicsScene* myPhysicsScene;
};

