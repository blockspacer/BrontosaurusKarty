#pragma once
class CSpotLight
{
public:
	CSpotLight();
	~CSpotLight();

	inline const CU::Vector3f& GetColor() const;
	inline float GetIntensity() const;
	inline float GetRange() const;


	inline void SetColor(const CU::Vector3f& aColor);
	inline void SetIntensity(const float aIntensity);
	inline void SetRange(const float aRange);

	inline void SetDirection(const CU::Vector3f aDirection);
	inline const CU::Vector3f& GetDirection() const;


private:
	CU::Vector3f myColor;
	CU::Vector3f myDirection;

	float myIntensity;
	float myRange;

};

inline const CU::Vector3f & CSpotLight::GetColor() const
{
	return myColor;
}

inline float CSpotLight::GetIntensity() const
{
	return myIntensity;
}

inline float CSpotLight::GetRange() const
{
	return myRange;
}

inline void CSpotLight::SetColor(const CU::Vector3f & aColor)
{
	myColor = aColor;
}

inline void CSpotLight::SetIntensity(const float aIntensity)
{
	myIntensity = aIntensity;
}

inline void CSpotLight::SetRange(const float aRange)
{
	myRange = aRange;
}