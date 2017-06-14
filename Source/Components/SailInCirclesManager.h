#pragma once
#include "SnurrComponent.h"

class CSailInCirclesComponent;

class CSailInCirclesManager
{
public:
	static void CreateInstance();
	static void Destroy();
	static CSailInCirclesManager& GetInstance();

	CSailInCirclesComponent* CreateComponent(const float aRPM, const float aVerticalRPM, const float aRadius, const float aVerticalAmplitude);
	CSnurrComponent* CreateSnurrComponent(const CU::Vector3f & aRotationsVector);

	void Update(const float aDeltaTime);

private:
	CSailInCirclesManager();
	~CSailInCirclesManager();

private:
	CU::GrowingArray<CSailInCirclesComponent*> mySailInCirclesComponent;
	CU::GrowingArray<CSnurrComponent*> mySnurrComponent;

	static CSailInCirclesManager* ourInstance;
	float myTimer;
};

