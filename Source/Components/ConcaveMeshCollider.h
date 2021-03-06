#pragma once
#include "ColliderComponent.h"

struct SConcaveMeshColliderData : SColliderData
{
	SConcaveMeshColliderData() { myType = eColliderType::eConcaveMesh; }
	const char* myPath;
};

class CConcaveMeshColliderComponent : public CColliderComponent
{
public:
	CConcaveMeshColliderComponent(const SConcaveMeshColliderData& aColliderData, Physics::CPhysicsScene* aScene);
	~CConcaveMeshColliderComponent();

	void ReInit(const CU::Vector3f& aScale);
	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	const SConcaveMeshColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eConcaveMesh; }

private:
	void Init(const CU::Vector3f& aScale);
	SConcaveMeshColliderData myData;
	Physics::CPhysicsScene* myScene;
};
