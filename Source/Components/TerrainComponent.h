#pragma once
namespace Component
{
	class CTerrainComponent : public CComponent
	{
	public:
		CTerrainComponent(const float aSpeedModifier);
		~CTerrainComponent();
		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
		bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
		void Destroy() override;
	private:
		float mySpeedModifier;
	};
}


