#pragma once
#include "../Game/NavigationSpline.h"

namespace Physics {
	class CPhysicsScene;
}

class CKartControllerComponent;
class CModelComponent;
class CGoalComponent;

class CKartControllerComponentManager
{
public:
	CKartControllerComponentManager();
	~CKartControllerComponentManager();

	CKartControllerComponent* CreateAndRegisterComponent(CModelComponent& aModelComponent, const short aControllerIndex = -1);

	void Update(const float aDeltaTime);
	void Init();


	void ShouldUpdate(const bool aShouldUpdate);

	void LoadNavigationSpline(const CU::CJsonValue& aJsonValue);

	const CNavigationSpline& GetNavigationSpline() const;
	const CU::Vector3f GetClosestSpinesDirection(const CU::Vector3f& aKartPosition);
	const int GetClosestSpinesIndex(const CU::Vector3f& aKartPosition);
	const SNavigationPoint* GetNavigationPoint(const int aIndex);
	inline void SetGoalComponent(CGoalComponent* aGoalComponent);
	void SetPhysiscsScene(Physics::CPhysicsScene* aPhysicsScene);
private:
	CU::GrowingArray<CKartControllerComponent*> myComponents;
	CGoalComponent* myGoalComponentPointer;
	Physics::CPhysicsScene* myPhysicsScene;
	bool myShouldUpdate;

	CNavigationSpline myNavigationSpline;
};

inline void CKartControllerComponentManager::SetGoalComponent(CGoalComponent* aGoalComponent)
{
	myGoalComponentPointer = aGoalComponent;
}