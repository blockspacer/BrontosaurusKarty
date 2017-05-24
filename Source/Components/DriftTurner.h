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
		void DoUpdate(const float aDeltaTime);
		CU::Matrix33f myTargetTurn;
	private:
		bool myHasBeenHit;
		float myHasBeenHitTotalTime;
	};
}

