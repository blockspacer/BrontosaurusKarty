#pragma once
class CSpeedHandlerComponent;
struct SBoostData;
enum class eBoostType;
class CSpeedHandlerManager
{
public:
	~CSpeedHandlerManager();

	static void CreateInstance();
	static void Destroy();
	static CSpeedHandlerManager* GetInstance();
	void Init();
	void LoadBoostData();
	const SBoostData* GetData(eBoostType aType) const;
	void Update(float aDeltaTime);
private:
	CSpeedHandlerManager();
	
private:
	static CSpeedHandlerManager* ourInstance;
	CU::GrowingArray<CSpeedHandlerComponent*> myComponents;
	CU::GrowingArray<SBoostData*> myBoostDataList;
};
