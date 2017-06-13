#pragma once


enum class eLerpCurve : char;


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

		float myTargetAngle;
		float myStartAngle;
		
		float myLerpTimer;
		bool shouldLerp;

		eLerpCurve myCurve;
	private:
		bool myHasBeenHit;
		float myHasBeenHitTotalTime;
	};
}

