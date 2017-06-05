#pragma once
#include "../Game/NavigationSpline.h"

namespace Physics {
	class CPhysicsScene;
}

class CKartControllerComponent;
class CModelComponent;
class CGoalComponent;

struct SParticipant;

class CKartControllerComponentManager
{
public:
	CKartControllerComponentManager();
	~CKartControllerComponentManager();

	CKartControllerComponent* CreateAndRegisterComponent(CModelComponent& aModelComponent, const short aControllerIndex = -1);
	CKartControllerComponent* CreateAndRegisterComponent(CModelComponent& aModelComponent, const SParticipant& aParticipant);

	void Update(const float aDeltaTime);
	void Init(Physics::CPhysicsScene* aPhysicsScene);

	void ShouldUpdate(const bool aShouldUpdate);

	void LoadNavigationSpline(const CU::CJsonValue& aJsonValue);

	const CNavigationSpline& GetNavigationSpline() const;
	const CU::Vector3f GetClosestSpinesDirection(const CU::Vector3f& aKartPosition);
	const int GetClosestSpinesIndex(const CU::Vector3f& aKartPosition);
	const SNavigationPoint* GetNavigationPoint(const int aIndex);
	inline void SetGoalComponent(CGoalComponent* aGoalComponent);
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