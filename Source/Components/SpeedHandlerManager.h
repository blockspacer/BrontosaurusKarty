#pragma once
class CSpeedHandlerComponent;
struct SBoostData;
class CSpeedHandlerManager
{
public:
	~CSpeedHandlerManager();

	static void CreateInstance();
	static void Destroy();
	static CSpeedHandlerManager* GetInstance();
	void Init();
	void LoadBoostData();
	const SBoostData* GetData(short aHashedName) const;
	void Update(float aDeltaTime);
	CSpeedHandlerComponent* CreateAndRegisterComponent();
private:
	CSpeedHandlerManager();
	
private:
	static CSpeedHandlerManager* ourInstance;
	CU::GrowingArray<CSpeedHandlerComponent*> myComponents;
	CU::GrowingArray<SBoostData*> myBoostDataList;
};
