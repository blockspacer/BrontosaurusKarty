#pragma once
#include "NavigationSpline.h"

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

	void LoadNavigationSpline(const CU::CJsonValue& aJsonValue);

	const CNavigationSpline& GetNavigationSpline() const;
	
private:
	CU::GrowingArray<CKartControllerComponent*> myComponents;
	Physics::CPhysicsScene* myPhysicsScene;
	bool myShouldUpdate;

	CNavigationSpline myNavigationSpline;
};
