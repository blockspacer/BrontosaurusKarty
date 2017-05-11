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

	void ShouldUpdate(const bool aShouldUpdate);

private:
	CU::GrowingArray<CKartControllerComponent*> myComponents;
	Physics::CPhysicsScene* myPhysicsScene;
	bool myShouldUpdate;
};
