#include "stdafx.h"
#include "DecalInstance.h"


CDecalInstance::CDecalInstance()
{
	SetSize(1.f, 1.f, 1.f);
}

CDecalInstance::~CDecalInstance()
{
}

void CDecalInstance::SetGBufferWeights(const CU::Vector4f& aWeights)
{
	myDecalData.decalGBufferBlends = aWeights;
}

void CDecalInstance::SetDecalIndex(const unsigned int aIndex)
{
	myDecalData.decalIndex = aIndex;
}

void CDecalInstance::SetActive(const bool aActive)
{
	myIsActive = aActive;
}

void CDecalInstance::SetSize(const float aWidth, const float aHeight, const float aDepth)
{
	myDecalData.decalProjection = CU::Matrix44f::CreateOrthogonalProjectionMatrixLH(-aDepth / 2.f, aDepth / 2.f, aWidth, aHeight);
}

CU::Matrix44f& CDecalInstance::GetTransformation()
{
	return myTransform;
}

const CU::Matrix44f& CDecalInstance::GetTransformation() const
{
	return myTransform;
}

const SDecalData& CDecalInstance::GetData() const
{
	return myDecalData;
}

bool CDecalInstance::GetIsActive() const
{
	return myIsActive;
}
