#include "stdafx.h"
#include "AnimationEventFactory.h"
#include "AnimationEvent.h"

#include "AnimationState.h"
#include "KartAnimator.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

#include "../CommonUtilities/WindowsHelper.h"
#include "CommonUtilities/JsonValue.h"


void PrintDocs()
{
	std::ofstream docFile("Json/Animation/types.txt");
	docFile << "Animation event types: " << std::endl << std::endl;

	ENUM_STRING_MACRO(JsonStrings, eBeginRight, eContinueRight, eFinnishRight, eBeginLeft, eContinueLeft, eFinnishLeft);
	for (const std::string& str : SJsonStrings::AnimationStates)
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

	ENUM_STRING_MACRO(JsonStrings, eBeginRight, eContinueRight, eFinnishRight, eBeginLeft, eContinueLeft, eFinnishLeft);
	CU::GrowingArray<std::string> filePaths = WindowsHelper::GetFilesInDirectory("Json\\Animation");

	filePaths.RemoveCyclic("types.txt");

	for (const std::string& filePath : filePaths)
	{
		CU::CJsonValue animationFile("Json\\Animation\\" + filePath);
		
		CU::CJsonValue animations = animationFile["animations"];
		for (int i = 0; i < animations.Size(); ++i)
		{
			int index = SJsonStrings::AnimationStates.Find(animations[i]["type"].GetString());
			if (index != SJsonStrings::AnimationStates.FoundNone)
			{
				const std::string& stateStr = animations[i]["state"].GetString();
				int state = SAnimationState::AnimationStates.Find(stateStr);
				if (state == SAnimationState::AnimationStates.FoundNone)
				{
					DL_MESSAGE_BOX("%s is not an animation state", stateStr.c_str());
					continue;
				}
				myAnimationEvents[static_cast<eEventType>(index)].state = static_cast<eAnimationState>(state);
				myAnimationEvents[static_cast<eEventType>(index)].start = animations[i]["start"].GetFloat();
				myAnimationEvents[static_cast<eEventType>(index)].end = animations[i]["end"].GetFloat();
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

CAnimationEvent CAnimationEventFactory::CreateEvent(const eEventType aType, CKartAnimator& aKartAnimator) const
{
	const SAnimationData& data = myAnimationEvents.at(aType);
	float end = data.end;
	float start = data.start;

	switch (aType)
	{
	case CAnimationEventFactory::eEventType::eBeginRight:
	case CAnimationEventFactory::eEventType::eFinnishRight:
	case CAnimationEventFactory::eEventType::eBeginLeft:
	case CAnimationEventFactory::eEventType::eFinnishLeft:
		return CAnimationEvent([start, end](float aTimer) -> bool { return aTimer + start < end; }, data.state, start, end);
	case CAnimationEventFactory::eEventType::eContinueRight:
		return CAnimationEvent([&aKartAnimator](float) -> bool { return aKartAnimator.IsTurningRight(); }, data.state, start, end);
	case CAnimationEventFactory::eEventType::eContinueLeft:
		return CAnimationEvent([&aKartAnimator](float) -> bool { return aKartAnimator.IsTurningLeft(); }, data.state, start, end);
	}

	return CAnimationEvent();
}
