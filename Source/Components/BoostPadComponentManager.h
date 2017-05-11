#pragma once
class CBoostPadComponent;
class CBoostPadComponentManager
{
public:
	CBoostPadComponentManager();
	~CBoostPadComponentManager();

	CBoostPadComponent* CreateAndRegisterComponent();
	void Update();
private:
	CU::GrowingArray<CBoostPadComponent*> myComponents;
};

