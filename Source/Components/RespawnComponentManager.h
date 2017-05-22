#pragma once
class CRespawnerComponent;
class CRespawnComponentManager
{
public:
	CRespawnComponentManager();
	~CRespawnComponentManager();

	void Update();
	CRespawnerComponent* CreateAndRegisterComponent();
private:
	CU::GrowingArray<CRespawnerComponent*> myComponents;
};

