#pragma once

class CAnimationEvent;
class CKartAnimator;

enum class eEventType
{
	eBeginRight,
	eContinueRight,
	eFinishRight,
	eBeginLeft,
	eContinueLeft,
	eFinishLeft,
};


class CAnimationEventFactory
{
public:
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
