#pragma once

class CAnimationEvent;
class CKartAnimator;

class CAnimationEventFactory
{
public:
	enum class eEventType
	{
		eBeginRight,
		eContinueRight,
		eFinnishRight,
		eBeginLeft,
		eContinueLeft,
		eFinnishLeft,
	};

	CAnimationEventFactory();
	~CAnimationEventFactory();

	static CAnimationEventFactory* GetInstance();

	bool LoadAnimationEvents();

	CAnimationEvent CreateEvent(const eEventType aType, CKartAnimator& aKartAnimator) const;

private:
	struct SAnimationData;
	std::map<eEventType, SAnimationData> myAnimationEvents;

	static CAnimationEventFactory* ourInstance;
};
