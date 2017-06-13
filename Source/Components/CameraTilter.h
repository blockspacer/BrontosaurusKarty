#pragma once
class CCameraTilter : public CComponent
{
public:
	CCameraTilter(Physics::CPhysicsScene* aPhysicsScene);
	~CCameraTilter();
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;

protected:
	void Update(const float aDt);
	Physics::CPhysicsScene* myPhysicsScene;
	float myTargetRotation;
	float myCurrentRotation;
};

