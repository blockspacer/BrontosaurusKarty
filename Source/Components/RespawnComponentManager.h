#pragma once
class CRespawnerComponent;
class CRespawnComponentManager
{
public:
	CRespawnComponentManager();
	~CRespawnComponentManager();

	void Update(float aDeltaTime);
	CRespawnerComponent* CreateAndRegisterComponent();
private:
	CU::GrowingArray<CRespawnerComponent*> myComponents;
};

