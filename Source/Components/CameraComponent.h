#pragma once
#include "Component.h"

namespace CU
{
	template <typename TYPE>
	class Matrix44;
	using Matrix44f = Matrix44<float>;

	class Camera;
}

class CCameraComponent : public CComponent
{
public:
	CCameraComponent();
	~CCameraComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void SetCamera(CU::Camera& aCamera);
	void Update(float aDeltaTime);
private:
	void Pitch(const float aPitch);
	bool GetLookat(CU::Vector3f& aLookat);
	bool GetPosition(CU::Vector3f& aPosition);

	CU::Matrix44f myKartOffset;
	CU::Matrix44f myCameraInterpolateTowardsMatrix;
	CU::Camera* myCamera;
	float myInterpolatingSpeed;
	bool myUnlocked;
};
