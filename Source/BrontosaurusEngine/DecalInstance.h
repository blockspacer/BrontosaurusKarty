#pragma once
#include "Lights.h"

class CDecalInstance
{
public:
	CDecalInstance();
	~CDecalInstance();

	void SetGBufferWeights(const CU::Vector4f& aWeights);
	void SetDecalIndex(const unsigned int aIndex);
	void SetActive(const bool aActive);

	void SetSize(const float aWidth, const float aHeight, const float aDepth);

	CU::Matrix44f& GetTransformation();
	const CU::Matrix44f& GetTransformation() const;
	const SDecalData& GetData() const;
	bool GetIsActive() const;

private:
	CU::Matrix44f myTransform;
	SDecalData myDecalData;
	bool myIsActive;
};
