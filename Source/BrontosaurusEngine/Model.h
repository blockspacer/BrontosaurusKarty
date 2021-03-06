#pragma once
#include "BufferStructs.h"

#include "../CommonUtilities/AABB.h"
#include "EffectsManager.h"

class CEffect;
class CSurface;
class CDXFramework;
struct ID3D11Buffer;
struct SVertexData;
struct SConstLayout;
class CLoaderModel;
class CLoaderMesh;

class CSceneAnimator;
class CModelInstance;
class CModelLoader;
class CPointLightInstance;

enum class eAnimationState;

namespace Lights
{
	struct SDirectionalLight;
	class matrix44;
}

namespace Render
{
	struct SEffectData;
}

namespace CU
{
	class Camera;
}

struct SLodData
{
	SLodData() { myLodLevel = myIndexCount = myVertexCount = 0; myVertexBuffer = myIndexBuffer = nullptr; }
	~SLodData() { /*SAFE_RELEASE(myVertexBuffer); SAFE_RELEASE(myIndexBuffer);*/ }
	
	ID3D11Buffer* myVertexBuffer;	  
	ID3D11Buffer* myIndexBuffer;	  

	SLODDistances myDistances;		
									  
	int myLodLevel;		
	unsigned int myVertexCount;		  
	unsigned int myIndexCount;
};

class CModel
{
	friend class CModelLoader;
	friend class CModelManager;

public:
	enum class eShaderStage
	{
		eVertex,
		eGeometry,
		ePixel,
		eLength
	};

	CModel();
	CModel(CModel&& aTemporary);
	CModel(const CModel& aCopy);
	~CModel();

	CModel& operator=(const CModel& aModel);
	CModel& operator=(CModel&& aModel);

	bool Initialize(CEffect* aEffect, CSurface* aSurface, const CU::GrowingArray<CLoaderMesh*>& aLoadedMesh);
	bool Initialize(CEffect* aEffect, CSurface* aSurface);

	void Render(SForwardRenderModelParams& aParamObj, const Render::SEffectData& anEffectData);
	void Render(SDeferredRenderModelParams& aParamObj, const Render::SEffectData& anEffectData);
	//void Render(SShadowRenderModelParams& aParamObj);


	void RenderInstanced(const bool aRenderDepth, ID3D11PixelShader* aDepthShader, const Render::SEffectData& anEffectData);
	void AddInstanceToRenderBuffer(const SDeferredRenderModelParams& aParamObj);

	inline bool GetInitialized() const;

	float GetRadius() const;

	CU::Matrix44f GetBoneTransform(const float aTime, const eAnimationState aAnimationState, const std::string& aBoneName);
	std::vector<CU::Matrix44f>& GetBones(float aTime, const eAnimationState aAnimationState, const bool aAnimationLooping);
	inline bool HasBones() const;
	inline bool HasAnimations() const;

	inline bool IsAlphaModel() const;
	inline const struct aiScene* GetScene() const;
	inline void SetScene(const struct aiScene* aScene);

	inline void AddRef();
	inline void RemoveRef();
	inline int GetRefCount() const;
	__forceinline const std::string& GetName() const;

	float GetAnimationDuration(const eAnimationState aAnimationState) const;
	bool GetAnimationStates(CU::GrowingArray<eAnimationState>& aAnimationStatesOut) const;
	void SetEffectType(const Render::SEffectData& aEffectData);
private:

	SLodData& GetCurrentLODModel(const CU::Vector3f& aModelPosition);

	void InitConstBuffers();
	bool InitBuffers(const CLoaderMesh * aLoadedMesh);
	void UpdateCBuffer(SForwardRenderModelParams& aParamObj);
	void UpdateCBuffer(SDeferredRenderModelParams& aParamObj);
	//void UpdateCBuffer(SShadowRenderModelParams& aParamObj);

	void UpdateInstanceBuffer(const unsigned int aStartIndex);

	static void BlendBones(const std::vector<CU::Matrix44f>& aBlendFrom, const std::vector<CU::Matrix44f>& aBlendTo, const float aLerpValue, std::vector<CU::Matrix44f>& aBlendOut);

private:
#ifdef _DEBUG
	std::string myFilePath;
#endif // _DEBUG

	std::map<eAnimationState, CSceneAnimator> mySceneAnimators;

	CU::GrowingArray<SLodData> myLODModels;
	CU::GrowingArray<SToWorldSpace> myInstanceBufferData;

	ID3D11Buffer* myCbuffer; // vertexBuffer
	ID3D11Buffer* myLightBuffer; // pixelbuffer
	ID3D11Buffer* myPixelBuffer;

	ID3D11Buffer* myInstanceBuffer;

	ID3D11Buffer* myBoneBuffer;
	
	CSceneAnimator* mySceneAnimator;
	CSceneAnimator* myBindposeSceneAnimator;

	//CEffect* myDeferredEffect;
	//CEffect* myForwardEffect;
	
	CSurface* mySurface;
	CDXFramework* myFramework;

	const struct aiScene* myScene;
	Render::SEffectData myEffectData;
	static const unsigned int ourMaxBoneBufferSize = 64u * sizeof(CU::Matrix44f);
	static const unsigned int ourMaxInstances = 512u;
	static const unsigned int ourMaxInstanceSize = ourMaxInstances * sizeof(SToWorldSpace);

	unsigned int myVertexSize;
	int myRefCount;
	
	float myRadius;

	std::atomic_bool myIsInitialized;
	bool myIsAlphaModel;
};

inline float CModel::GetRadius() const
{
	return myRadius;
}

inline const aiScene* CModel::GetScene() const
{
	return myScene;
}

inline void CModel::SetScene(const aiScene* aScene)
{
	myScene = aScene;
}

inline bool CModel::HasAnimations() const
{
	return mySceneAnimator != nullptr;
}

inline void CModel::AddRef()
{
	myRefCount++;
}

inline void CModel::RemoveRef()
{
	myRefCount--;
}

inline int CModel::GetRefCount() const
{
	return myRefCount;
}

inline bool CModel::HasBones() const
{
	return myBoneBuffer != nullptr;
}

inline bool CModel::IsAlphaModel() const
{
	return myIsAlphaModel;
}

inline bool CModel::GetInitialized() const
{
	return myIsInitialized;
}

const std::string& CModel::GetName() const
{
#ifdef _DEBUG
	return myFilePath;
#else // !_DEBUG
	return "";
#endif // _DEBUG
}
