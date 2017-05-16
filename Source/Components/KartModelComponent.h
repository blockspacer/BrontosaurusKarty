#pragma once
namespace Physics {
	class CPhysicsScene;
}

namespace Component
{
	class CKartModelComponent : public CComponent
	{
	public:
		explicit CKartModelComponent(Physics::CPhysicsScene*);
		~CKartModelComponent();
		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
		bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
		void Destroy() override;

		void Init(Physics::CPhysicsScene* aPhysicsScene);
	protected:
		enum class AxisPos
		{
			RightBack,
			RightFront,
			LeftBack,
			LeftFront,
			Size
		};

		void ClearHeight();
		void ClearHeight(const int anIndex);
		
		void ClearSpeed();

		float GetHeightSpeed(int anIndex) const;
		void SetHeight(int anIndex, float aHeight, float aDeltaTime);

		void NormalizeRotation(const float aDeltaTime);
		void Update(const float aFloat);

		void Reset();

		struct
		{
			float width = 1.f;
			float length = 1.f;
		} myAxisDescription;

		Physics::CPhysicsScene* myPhysicsScene;

		float myPreviousHeight[static_cast<int>(AxisPos::Size)];
		float myCurrentHeight[static_cast<int>(AxisPos::Size)];

		float myAxisSpeed[static_cast<int>(AxisPos::Size)];

		bool myIsOnGround;

	};
}

