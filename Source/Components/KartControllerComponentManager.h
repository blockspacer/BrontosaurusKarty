#pragma once
#include "NavigationSpline.h"

namespace Physics {
	class CPhysicsScene;
}

class CKartControllerComponent;
class CModelComponent;

class CKartControllerComponentManager
{
public:
	CKartControllerComponentManager();
	~CKartControllerComponentManager();

	CKartControllerComponent* CreateAndRegisterComponent(const short aControllerIndex = -1);

	void Update(const float aDeltaTime);
	void Init(Physics::CPhysicsScene* aPhysicsScene);

	void ShouldUpdate(const bool aShouldUpdate);

	void LoadNavigationSpline(const CU::CJsonValue& aJsonValue);

	const CNavigationSpline& GetNavigationSpline() const;
	const CU::Vector3f GetClosestSpinesDirection(const CU::Vector3f& aKartPosition);
	const int GetClosestSpinesIndex(const CU::Vector3f& aKartPosition);
	const SNavigationPoint* GetNavigationPoint(const int aIndex);
private:
	CU::GrowingArray<CKartControllerComponent*> myComponents;
	Physics::CPhysicsScene* myPhysicsScene;
	bool myShouldUpdate;

	CNavigationSpline myNavigationSpline;
};
