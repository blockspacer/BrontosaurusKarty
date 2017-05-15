#pragma once
namespace Component
{
	class CSmoothRotater : public CComponent
	{
	public:
		struct SRotationData
		{
			CU::Matrix44f target;
			float stepSize;
		};
		CSmoothRotater();
		~CSmoothRotater();

		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
		bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
		void Destroy() override;
	protected:
		void DoRotation(const SRotationData& aData);
	};
}

