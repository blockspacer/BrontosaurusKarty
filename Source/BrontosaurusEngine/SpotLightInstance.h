#pragma once
#include "Lights.h"
enum class ePointLights;

class CSpotLightInstance
{
public:
	CSpotLightInstance();
	~CSpotLightInstance();

	inline void SetPosition(const CU::Vector3f& aPos);
	inline void SetRange(const float aRange);
	inline void SetInstensity(const float aIntencity);
	inline void SetColor(const CU::Vector3f & aColor);
	inline void SetDirection(const CU::Vector3f & aDirection);
	inline void SetSpotAngle(const float aSpotAngle);
	inline void SetRotation(const CU::Matrix33f& aRotation);

	inline const Lights::SSpotLight& GetData() const;
	inline const CU::Matrix33f& GetRotation();

	inline bool GetIsActive() const;
	inline void SetActive(const bool aIsActive);
private:
	Lights::SSpotLight myData;
	CU::Matrix33f myRotation;
	bool myIsActive;
};


inline void CSpotLightInstance::SetPosition(const CU::Vector3f& aPos)
{
	myData.position = aPos;
}

inline void CSpotLightInstance::SetRange(const float aRange)
{
	myData.range = aRange;
}

inline void CSpotLightInstance::SetInstensity(const float aIntencity)
{
	myData.intensity = aIntencity;
}

inline void CSpotLightInstance::SetColor(const CU::Vector3f & aColor)
{
	myData.color = aColor;
}

inline void CSpotLightInstance::SetDirection(const CU::Vector3f & aDirection)
{
	myData.direction = aDirection;
}

inline void CSpotLightInstance::SetSpotAngle(const float aSpotAngle)
{
	myData.spotAngle = aSpotAngle;
}

inline void CSpotLightInstance::SetRotation(const CU::Matrix33f & aRotation)
{
	myRotation = aRotation;
	myRotation *= myRotation.CreateRotateAroundY(3.141592f);
	myData.direction = myRotation.myForwardVector.GetNormalized();
}

inline const Lights::SSpotLight & CSpotLightInstance::GetData() const
{
	return myData;
}

inline const CU::Matrix33f & CSpotLightInstance::GetRotation()
{
	return myRotation;
}

inline bool CSpotLightInstance::GetIsActive() const
{
	return myIsActive;
}

inline void CSpotLightInstance::SetActive(const bool aIsActive)
{
	myIsActive = aIsActive;
}