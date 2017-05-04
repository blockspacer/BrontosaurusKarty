#pragma once
class CKartComponent;
class CKartComponentManager
{
public:
	CKartComponentManager();
	~CKartComponentManager();

	CKartComponent* CreateComponent();
	void Update(float aDeltaTime);
private:
	CU::GrowingArray<CKartComponent*> myComponents;
};

