#pragma once

class CExplosionComponent;

class CExplosionComponentManager
{
public:
	CExplosionComponentManager();
	~CExplosionComponentManager();

	CExplosionComponent* CreateExplosionComponent();

	void Update(const float aDeltaTime);

private:
	CU::GrowingArray<CExplosionComponent*> myComponents;
};

