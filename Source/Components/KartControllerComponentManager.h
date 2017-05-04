#pragma once

class CKartControllerComponent;

class CKartControllerComponentManager
{
public:
	CKartControllerComponentManager();
	~CKartControllerComponentManager();

	CKartControllerComponent* CreateAndRegisterComponent();

	void Update(const float aDeltaTime);


private:
	CU::GrowingArray<CKartControllerComponent*> myComponents;

};

