#pragma once

class CKartControllerComponent;

class CKartControllerComponentManager
{
public:
	CKartControllerComponentManager();
	~CKartControllerComponentManager();

	CKartControllerComponent* CreateAndRegisterComponent();

	void Update(const float aDeltaTime);
	void ShouldUpdate(const bool aShouldUpdate);

private:
	CU::GrowingArray<CKartControllerComponent*> myComponents;
	bool myShouldUpdate;
};
