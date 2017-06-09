#pragma once
#include "State.h"
#include "../CommonUtilities/InputListener.h"
#include "MenuManager.h"
#include "SParticipant.h"

namespace CU
{
	class CJsonValue;
}

class CSplitScreenSelection : public State
{

	struct STextInput
	{
		STextInput() : myInputIsValid(true), myTextInstance(nullptr)
		{
		}

		bool myInputIsValid;
		CTextInstance* myTextInstance;
	};


public:
	CSplitScreenSelection(StateStack & aStateStack);
	~CSplitScreenSelection();

	CU::eInputReturn RecieveInput(const CU::SInputMessage & aInputMessage) override;

	// Inherited via State
	void Init() override;
	eStateStatus Update(const CU::Time & aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	void MenuLoad(const std::string& aFile);
	void LoadElement(const CU::CJsonValue& aJsonValue, const std::string& aFolderpath);
	static eAlignment LoadAlignment(const CU::CJsonValue& aJsonValue);
private:
	void RightChar();
	void LeftChar();


	bool PushLevel(const std::string& aString);
	bool BackToMenu(const std::string& aString);
	CMenuManager myMenuManager;
	CU::GrowingArray<STextInput> myTextInputs;
	CU::GrowingArray<SParticipant> myPlayers;
	CU::GrowingArray<CSpriteInstance*> myCharacterSprites;

	CU::StaticArray<SParticipant::eInputDevice,4> myPlayerInputDevices;
	bool myHasKeyboardResponded;
	bool myShowStateBelow;
};

