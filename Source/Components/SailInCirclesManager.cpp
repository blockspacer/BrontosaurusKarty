#include "stdafx.h"
#include "SailInCirclesManager.h"
#include "SailInCirclesComponent.h"

CSailInCirclesManager* CSailInCirclesManager::ourInstance = nullptr;

CSailInCirclesManager::CSailInCirclesManager()
{
	myTimer = 0.0f;
	mySailInCirclesComponent.Init(8);
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

CSailInCirclesComponent* CSailInCirclesManager::CreateComponent(const float aRPM, const float aRadius)
{
	mySailInCirclesComponent.Add(new CSailInCirclesComponent(aRPM, aRadius));
	CComponentManager::GetInstance().RegisterComponent(mySailInCirclesComponent.GetLast());
	return mySailInCirclesComponent.GetLast();
}

void CSailInCirclesManager::Update(const float aDeltaTime)
{
	myTimer += aDeltaTime;

	for (CSailInCirclesComponent* component : mySailInCirclesComponent)
	{
		component->Update(myTimer);
	}
}
