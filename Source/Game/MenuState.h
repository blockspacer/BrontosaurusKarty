#pragma once
#include "../StateStack/State.h"
#include "../Game/MenuManager.h"
#include "../ThreadedPostmaster/Subscriber.h"

namespace CU 
{
	class CJsonValue;
}

struct STextInput
{
	STextInput(): myInputIsValid(true), myTextInstance(nullptr)
	{
	}

	bool myInputIsValid;
	CTextInstance* myTextInstance;
};

class CMenuState :public State, Postmaster::ISubscriber
{
public:

	CMenuState(StateStack& aStateStack, std::string aFile);
	~CMenuState();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;

	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	bool GetLetThroughRender() const override;
	bool GetLetThroughUpdate() const override;

	CU::eInputReturn RecieveInput(const CU::SInputMessage& aInputMessage) override;

	eMessageReturn DoEvent(const KeyCharPressed& aCharPressed) override;
	eMessageReturn DoEvent(const CConectedMessage& aCharPressed) override;
	eMessageReturn DoEvent(const CLoadLevelMessage& aLoadLevelMessage) override;

private:
	static eAlignment LoadAlignment(const CU::CJsonValue& aJsonValue);
	void LoadElement(const CU::CJsonValue& aJsonValue, const std::string& aFolderpath);
	void MenuLoad(const std::string& aFile);
	bool PushMenu(std::string aMenu);

	static bool ExitGame(std::string notUsed);
	bool PushSplitScreenSelection();
	bool PopMenues(std::string aNumberOfMenues);
	bool PushLevel(std::string aLevelIndexString);
	bool SetCurrentTextInput(std::string aTexINputIndex);

	CU::GrowingArray<STextInput> myTextInputs;
	int myCurrentTextInput;

	bool myShowStateBelow;
	CSpriteInstance* myPointerSprite;

	CMenuManager myManager;
	bool myIsInFocus;
	std::wstring myThisComputersIP;
	static char ourMenuesToPop;

	bool myBlinkeyBool;
	float myBlinkeyTimer;

	std::string myName;
	std::string myIp;
};

inline bool CMenuState::GetLetThroughRender() const
{
	return myShowStateBelow;
}

inline bool CMenuState::GetLetThroughUpdate() const
{
	return false;
}

