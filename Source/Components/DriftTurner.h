#pragma once
namespace Component
{
	class CDriftTurner : public CComponent
	{
	public:
		CDriftTurner();
		~CDriftTurner();
		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
		bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
		void Destroy() override;
	protected:
		void DoUpdate(const float aFloat);
		CU::Matrix33f myTargetTurn;
	};
}

