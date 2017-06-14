#include "stdafx.h"
#include "SailInCirclesManager.h"
#include "SailInCirclesComponent.h"

CSailInCirclesManager* CSailInCirclesManager::ourInstance = nullptr;

CSailInCirclesManager::CSailInCirclesManager()
{
	myTimer = 0.0f;
	mySailInCirclesComponent.Init(16);
	mySnurrComponent.Init(16);
}

CSailInCirclesManager::~CSailInCirclesManager()
{
}

void CSailInCirclesManager::CreateInstance()
{
	ourInstance = new CSailInCirclesManager();
}

void CSailInCirclesManager::Destroy()
{
	delete ourInstance;
	ourInstance = nullptr;
}

CSailInCirclesManager& CSailInCirclesManager::GetInstance()
{
	assert(ourInstance);
	return *ourInstance;
}

CSailInCirclesComponent* CSailInCirclesManager::CreateComponent(const float aRPM, const float aVerticalRPM, const float aRadius, const float aVerticalAmplitude)
{
	mySailInCirclesComponent.Add(new CSailInCirclesComponent(aRPM, aVerticalRPM, aRadius, aVerticalAmplitude));
	CComponentManager::GetInstance().RegisterComponent(mySailInCirclesComponent.GetLast());
	return mySailInCirclesComponent.GetLast();
}

CSnurrComponent* CSailInCirclesManager::CreateSnurrComponent(const CU::Vector3f & aRotationsVector)
{
	mySnurrComponent.Add(new CSnurrComponent(aRotationsVector));
	CComponentManager::GetInstance().RegisterComponent(mySnurrComponent.GetLast());
	return mySnurrComponent.GetLast();
}

void CSailInCirclesManager::Update(const float aDeltaTime)
{
	myTimer += aDeltaTime;

	for (CSnurrComponent* component : mySnurrComponent)
	{
		component->Update(aDeltaTime);
	}

	for (CSailInCirclesComponent* component : mySailInCirclesComponent)
	{
		component->Update(myTimer);
	}
}
