#pragma once
#include "RenderCamera.h"
#include "..\CommonUtilities\AABB.h"
#include "ShadowBuffers.h"

class CModelInstance;
using InstanceID = unsigned int;

class CShadowMap
{
public:
	CShadowMap();
	~CShadowMap();
	
	void Init(unsigned int aShadowMapSize);
	void Render(const CU::GrowingArray<CModelInstance*, InstanceID>& aModelList);
	void SetBoundingBox(const CU::Vector3f& aCenterPosition, const CU::Vector3f& aExtents);
	void SetDirection(const CU::Vector3f& aLightDirection);
	void SendToRenderer();
	CRenderPackage & GetShadowBuffer();

	inline bool GetIfFinishedBake();


private:
	inline void FinishedBake();
	void CalculateFrustum();

private:
	CRenderCamera myRenderCamera;
	CU::Vector3f myLightDirection;
	CU::AABB myBoundingBox;

	SBakedShadowBuffer myData;
	bool myHasBaked;
};

inline bool CShadowMap::GetIfFinishedBake()
{
	return myHasBaked;
}

inline void CShadowMap::FinishedBake()
{
	myHasBaked = true;
}