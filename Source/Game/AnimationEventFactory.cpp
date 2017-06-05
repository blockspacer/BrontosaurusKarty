#include "stdafx.h"
#include "AnimationEventFactory.h"
#include "AnimationEvent.h"

#include "AnimationState.h"
#include "KartAnimator.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

#define DECLARE_ANIMATION_ENUM_AND_STRINGS_TWO				\
ENUM_STRING_MACRO(JsonStrings,								\
eBeginRight, eContinueRight, eFinishRight,					\
eBeginLeft, eContinueLeft, eFinishLeft,						\
eBeginBreak, eContinueBreak, eFinishBreak,					\
eBeginAccelerate, eContinueAccelerate, eFinishAccelerate,	\
eBeginBoost, eContinueBoost, eFinishBoost)

#include "../CommonUtilities/WindowsHelper.h"
#include "CommonUtilities/JsonValue.h"


void PrintDocs()
{
	std::ofstream docFile("Json/Animation/types.txt");
	docFile << "Animation event types: " << std::endl << std::endl;

	DECLARE_ANIMATION_ENUM_AND_STRINGS_TWO;
	static_assert(sizeof(SJsonStrings) != sizeof(SAnimationState), "can't have same number of animation states and json strings for animations");
	for (const std::string& str : locJsonStrings.AnimationStates)
	{
		docFile << str << std::endl;
	}
	docFile.close();
}

struct CAnimationEventFactory::SAnimationData
{
	eAnimationState state;
	float start;
	float end;
};

CAnimationEventFactory* CAnimationEventFactory::ourInstance = nullptr;

CAnimationEventFactory::CAnimationEventFactory()
{
	assert(ourInstance == nullptr);
	ourInstance = this;
	PrintDocs();

	CU::GrowingArray<std::string> filePaths = WindowsHelper::GetFilesInDirectory("Json\\Animation");

	filePaths.RemoveCyclic("types.txt");
	DECLARE_ANIMATION_ENUM_AND_STRINGS_TWO;

	for (const std::string& filePath : filePaths)
	{
		if (filePath.rfind(".json") == std::string::npos) continue;

		CU::CJsonValue animationFile("Json\\Animation\\" + filePath);

		float frameRate = animationFile["framesPerSecond"].GetFloat();
		if (frameRate == 0.f)
		{
			DL_MESSAGE_BOX("Animation frame rate set to 0\n%s", filePath.c_str());
			continue;
		}
		CU::CJsonValue animations = animationFile["animations"];
		for (int i = 0; i < animations.Size(); ++i)
		{
			int index = locJsonStrings.AnimationStates.Find(animations[i]["type"].GetString());
			if (index != locJsonStrings.AnimationStates.FoundNone)
			{
				const std::string& stateStr = animations[i]["state"].GetString();
				int state = locAnimationState.AnimationStates.Find(stateStr);
				if (state == locAnimationState.AnimationStates.FoundNone)
				{
					DL_MESSAGE_BOX("%s is not an animation state", stateStr.c_str());
					continue;
				}
				eEventType type = static_cast<eEventType>(index);
				myAnimationEvents[type].state = static_cast<eAnimationState>(state);
				myAnimationEvents[type].start = animations[i]["start"].GetFloat() / frameRate;
				myAnimationEvents[type].end = animations[i]["end"].GetFloat() / frameRate;
			}
		}
	}
}

CAnimationEventFactory::~CAnimationEventFactory()
{
	assert(ourInstance == this);
	ourInstance = nullptr;
}

CAnimationEventFactory* CAnimationEventFactory::GetInstance()
{
	return ourInstance;
}

bool CAnimationEventFactory::LoadAnimationEvents()
{
	return false;
}

CAnimationEvent CAnimationEventFactory::CreateEvent(const eEventType aType, const CKartAnimator& aKartAnimator) const
{
	const SAnimationData& data = myAnimationEvents.at(aType);
	float end = data.end;
	float start = data.start;

	switch (aType)
	{
	case eEventType::eBeginRight:
	case eEventType::eFinishRight:
	case eEventType::eBeginBoost:
	case eEventType::eFinishBoost:
	case eEventType::eBeginLeft:
	case eEventType::eFinishLeft:
	case eEventType::eBeginBreak:
	case eEventType::eFinishBreak:
	case eEventType::eBeginAccelerate:
	case eEventType::eFinishAccelerate:
		return CAnimationEvent([start, end](float aTimer) -> bool { return aTimer + start < end; }, data.state, start, end);
	case eEventType::eContinueRight:
		return CAnimationEvent([&aKartAnimator](float) -> bool { return aKartAnimator.IsTurningRight(); }, data.state, start, end);
	case eEventType::eContinueLeft:
		return CAnimationEvent([&aKartAnimator](float) -> bool { return aKartAnimator.IsTurningLeft(); }, data.state, start, end);
	case eEventType::eContinueBreak:
		return CAnimationEvent([&aKartAnimator](float) -> bool { return aKartAnimator.IsBreaking(); }, data.state, start, end);
	case eEventType::eContinueAccelerate:
		return CAnimationEvent([/*&aKartAnimator*/start, end](float aTimer) -> bool {
			DL_PRINT("continue acc, timer: %f < %f", aTimer, end - start); return aTimer + start < end;/*aKartAnimator.IsAccelerating();*/
		}, data.state, start, end);
	case eEventType::eContinueBoost:
		return CAnimationEvent([&aKartAnimator](float) -> bool { return aKartAnimator.IsBoosting(); }, data.state, start, end);
	}

	return CAnimationEvent();
}
