#pragma once

class CGameObjectManager;
class CExplosionComponentManager;
class CColliderComponentManager;

class CBlueShellBehaviourComponent:public CComponent
{
public:
	CBlueShellBehaviourComponent(CU::GrowingArray<CGameObject*>& aListOfKartObjects, CGameObjectManager* aGameObjectManager, CExplosionComponentManager* aExplosionManager, CColliderComponentManager* aColliderManager);
	~CBlueShellBehaviourComponent();

	void Update(const float aDeltaTime);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::GrowingArray<CGameObject*>* myKartObjects;

	CGameObject* myExplosion;

	CGameObject* myUser;

	CU::Vector3f myVelocity;
	CU::Vector3f myDropSpeed;
	float myAboveHeight;
	float myTeleportDelay;
	float myElapsedTime;
	float mySpeed;
	float myDecendSpeed;

	bool myIsActive;
};

