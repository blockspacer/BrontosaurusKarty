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
	GetParent()->GetLocalTransform().RotateAroundAxis(myRotationVector.x * aDeltaTime, CU::Axees::X);
	GetParent()->GetLocalTransform().RotateAroundAxis(myRotationVector.y * aDeltaTime, CU::Axees::Y);
	GetParent()->GetLocalTransform().RotateAroundAxis(myRotationVector.z * aDeltaTime, CU::Axees::Z);

	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}
