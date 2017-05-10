#pragma once
#include "ColliderComponent.h"

struct SConcaveMeshColliderData : SColliderData
{
	SConcaveMeshColliderData() { myType = eColliderType::eMesh; }
	const char* myPath;
};

class CConcaveMeshColliderComponent : public CColliderComponent
{
public:
	CConcaveMeshColliderComponent(const SConcaveMeshColliderData& aColliderData, Physics::CPhysicsScene* aScene);
	~CConcaveMeshColliderComponent();

	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	const SConcaveMeshColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eMesh; }

private:
	void Init(const CU::Vector3f& aScale);
	SConcaveMeshColliderData myData;
	Physics::CPhysicsScene* myScene;
};
