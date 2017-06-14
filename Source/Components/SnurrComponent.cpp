#include "stdafx.h"
#include "SnurrComponent.h"


CSnurrComponent::CSnurrComponent(const CU::Vector3f & aRotationVector)
{
	myRotationVector = aRotationVector;
}

CSnurrComponent::~CSnurrComponent()
{
}

void CSnurrComponent::Update(const float aDeltaTime)
{
	GetParent()->GetLocalTransform().Rotate(myRotationVector.x * aDeltaTime, CU::Axees::X);
	GetParent()->GetLocalTransform().Rotate(myRotationVector.y * aDeltaTime, CU::Axees::Y);
	GetParent()->GetLocalTransform().Rotate(myRotationVector.z * aDeltaTime, CU::Axees::Z);

	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}
