#pragma once

#define ENUM_STRING_MACRO(name, ...)											\
enum class e##name { __VA_ARGS__, eLength };									\
constexpr int name##hiddenlength = static_cast<int>(e##name::eLength);			\
using S##name = TAnimationState<name##hiddenlength>;							\
static S##name loc##name(#__VA_ARGS__);

template<int NumStates>
struct TAnimationState
{
	TAnimationState(const char* aCommaSeperatedString);
	CU::StaticArray<std::string, NumStates> AnimationStates;
};

//template <int NumStates>
//CU::StaticArray<std::string, NumStates> TAnimationState<NumStates>::AnimationStates;

template<int NumStates>
inline TAnimationState<NumStates>::TAnimationState(const char* aCommaSeperatedString)
{
	std::string subString = aCommaSeperatedString;
	for (int i = 0; i < NumStates; ++i)
	{
		size_t nextComma = subString.find(',');
		if (nextComma == std::string::npos)
		{
			AnimationStates[i] = subString;
			break;
		}

		AnimationStates[i] = subString.substr(0, nextComma);
		subString = subString.substr(nextComma + 2);
	}
}

#define DECLARE_ANIMATION_ENUM_AND_STRINGS \
ENUM_STRING_MACRO(AnimationState, idle01, boost01, turnRight01, turnLeft01, accelerate01, break01, shot01, invisible, none)
