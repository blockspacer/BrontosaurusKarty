#pragma once
#include "Component.h"
class CSnurrComponent :	public CComponent
{
public:
	CSnurrComponent(const CU::Vector3f & aRotationVector);
	~CSnurrComponent();

	
	void Update(const float aDeltaTime);

private:
	CU::Vector3f myRotationVector;
};

