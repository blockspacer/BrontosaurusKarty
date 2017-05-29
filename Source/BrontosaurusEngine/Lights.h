#pragma once

#define NUMBER_OF_POINTLIGHTS 8 

enum class ePointLights
{
	eWhite,
	eSuperWhite,
	eRed,
	eBlue,
	eVoid,
	eGreen
};


namespace Lights
{
	struct SDirectionalLight
	{
		CU::Vector3f color = CU::Vector3f::One;
		float intensity = 1.0f;
		CU::Vector3f direction;
		int shadowIndex;
	};

	struct SPointLight
	{
		CU::Vector3f color = CU::Vector3f::One;
		float intensity;
		CU::Vector3f position;
		float range;
	};

	struct SSpotLight
	{
		CU::Vector3f color = CU::Vector3f::One;
		float intensity = 1.0f;;
		CU::Vector3f position;
		float range = 100.f;
		CU::Vector3f direction;
		float spotAngle = 3.141592f * 0.25f;
	};

	struct SLightsBuffer
	{
		CU::Vector4f myCameraPos;
		SDirectionalLight myDirectionalLight;
		SPointLight myPointLights[NUMBER_OF_POINTLIGHTS];
		CU::Vector4f highlightColor;
		unsigned int myNumberOfLights;
		unsigned int cubeMap_mipCount;
		
		float trash[2];
	};
}