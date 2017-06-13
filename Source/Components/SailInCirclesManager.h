#pragma once

class CSailInCirclesComponent;

class CSailInCirclesManager
{
public:
	static void CreateInstance();
	static void Destroy();
	static CSailInCirclesManager& GetInstance();

	CSailInCirclesComponent* CreateComponent(const float aRPM, const float aRadius);
	void Update(const float aDeltaTime);

private:
	CSailInCirclesManager();
	~CSailInCirclesManager();

private:
	CU::GrowingArray<CSailInCirclesComponent*> mySailInCirclesComponent;
	static CSailInCirclesManager* ourInstance;
	float myTimer;
};
