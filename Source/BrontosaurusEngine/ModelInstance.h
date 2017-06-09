#pragma once
#include "Texture.h"

enum class eAnimationState;

namespace CU
{
	template <typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;

	struct Sphere;
}

namespace Lights
{
	struct SDirectionalLight;
}

class CModel;
class CRenderCamera;
class CPointLightInstance;
class CSceneAnimator;

class CModelInstance
{
public:
	typedef int ModelId;

	friend class CModelManager;
	friend class CModelComponent;

public:
	CModelInstance(const std::string& aModelPath);
	CModelInstance(const CModelInstance& aCopy) = delete;
	~CModelInstance();

	bool ShouldRender();
	CU::Sphere GetModelBoundingSphere();

	void RenderDeferred();
	void RenderDeferred(CRenderCamera & aRenderToCamera);
	void RenderForward(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList);
	void RenderForward(Lights::SDirectionalLight* aLight, CU::VectorOnStack<CPointLightInstance, 8>& aPointLightList, CRenderCamera& aRenderToCamera);

	void Update(const CU::Time aDeltaTime);

	void SetTransformation(const CU::Matrix44f& aTransformation);
	void SetPosition(CU::Vector3f aPosition);
	void Rotate(float aRotation, CU::Axees aAxis) { myTransformation.Rotate(aRotation, aAxis); }

	float GetAnimationDuration(const eAnimationState aAnimationState) const;
	void SetAnimationManualUpdate(const bool aShouldManuallyUpdate);
	void SetAnimation(const eAnimationState aAnimationKey);
	void SetAnimationLerpie(const float aLerpValue);
	void SetNextAnimation(const eAnimationState aAnimationKey);
	void SetAnimationLooping(const bool aValue);
	void SetAnimationCounter(const float aValue);
	void ResetAnimation();
	eAnimationState GetAnimationState() const;
	float GetAnimationCounter() const;
	bool GetAnimationStates(CU::GrowingArray<eAnimationState>& aAnimationStatesOut) const;
	const std::string& GetFilePath() const;

	void SetHighlightIntencity(const float aHighlightIntencity);
	void SetHighlight(const CU::Vector4f& aColor, float anIntensivit);

	inline const CU::Matrix44f& GetTransformation() const;
	inline const CU::Matrix44f& GetLastFrameTransformation() const;
	inline void SetVisibility(const bool aIsVisible);
	inline bool GetVisibility();
	inline ModelId GetModelID();
	inline void SetIsShadowCasting(const bool aShouldCastShadows);
	inline bool GetIsShadowCasting() const;
	void SetVertexStreamData(const std::string& aStreamPath);
private:
	CU::Matrix44f myTransformation;
	CU::Matrix44f myLastFrame;

	CU::Vector4f myHighlightColor;

	eAnimationState myCurrentAnimation;
	eAnimationState myNextAnimation;

	ModelId myModel;
	
	float myAnimationLerpie;
	float myAnimationCounter;
	float myHighlightIntencity;
	
	bool myIsVisible;
	bool myHasAnimations;
	bool myAnimationLooping;
	bool myShouldManuallyUpdate;

	bool myIsShadowCasting;
	CTexture* myVertexTexture;
};

inline const CU::Matrix44f& CModelInstance::GetTransformation() const
{
	return myTransformation;
}

inline const CU::Matrix44f& CModelInstance::GetLastFrameTransformation() const
{
	return myLastFrame;
}

inline void CModelInstance::SetVisibility(const bool aIsVisible)
{
	myIsVisible = aIsVisible;
}

inline int CModelInstance::GetModelID()
{
	return myModel;
}

inline void CModelInstance::SetIsShadowCasting(const bool aShouldCastShadows)
{
	myIsShadowCasting = aShouldCastShadows;
}

inline bool CModelInstance::GetIsShadowCasting() const
{
	return myIsShadowCasting;
}

inline bool CModelInstance::GetVisibility()
{
	return myIsVisible;
}
