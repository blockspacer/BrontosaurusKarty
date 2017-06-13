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

	struct GUIPart
	{
		void Delete()
		{
			delete LeftArrow;
			LeftArrow = nullptr;
			delete RightArrow;
			RightArrow = nullptr;
			delete NameTag;
			NameTag = nullptr;
			delete JoinSprite;
			JoinSprite = nullptr;
			delete ReadySprite;
			ReadySprite = nullptr;
		}
		CSpriteInstance* LeftArrow;
		CSpriteInstance* RightArrow;
		CSpriteInstance* NameTag;
		CSpriteInstance* JoinSprite;
		CSpriteInstance* ReadySprite;
		CU::Vector2f LeftArrowOriginPosition;
		CU::Vector2f RightArrowOriginPosition;
		bool hasJoined;
		float timer = 0.0f;
		void Update(float aDeltaTime)
		{
			float speed = 0.33f;
			if (LeftArrow->GetPosition().x < LeftArrowOriginPosition.x)
			{
				LeftArrow->SetPosition(CU::Vector2f(LeftArrow->GetPosition().x + speed * aDeltaTime, LeftArrow->GetPosition().y));
			}
			if (RightArrow->GetPosition().x > RightArrowOriginPosition.x)
			{
				RightArrow->SetPosition(CU::Vector2f(RightArrow->GetPosition().x - speed * aDeltaTime, RightArrow->GetPosition().y));
			}
		}
		void Redner()
		{
			if (hasJoined == false)
			{
				JoinSprite->RenderToGUI(L"__Menu");
				return;
			}
			LeftArrow->RenderToGUI(L"__Menu");
			RightArrow->RenderToGUI(L"__Menu");
			NameTag->RenderToGUI(L"__Menu");
		}
	};

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
	void RightChar(SParticipant& aParticipant);
	void LeftChar(SParticipant& aParticipant);

	bool PushLevel(const std::string& aString);
	bool BackToMenu(const std::string& aString);

	float myLastJoyX;


	CSpriteInstance* myAllReadySprite;

	CMenuManager myMenuManager;
	CU::GrowingArray<STextInput> myTextInputs;
	CU::GrowingArray<SParticipant> myPlayers;
	CU::GrowingArray<CSpriteInstance*> myCharacterSprites;
	CU::GrowingArray<GUIPart> myGUIParts;

	CU::StaticArray<SParticipant::eInputDevice,4> myPlayerInputDevices;
	bool myHasKeyboardResponded;
	bool myShowStateBelow;
	bool myIsInFocus;
	bool myRenderAllReady;
};

