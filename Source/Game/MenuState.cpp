#include "stdafx.h"
#include "MenuState.h"
#include "InputMessage.h"
#include "EInputReturn.h"
#include "EMouseButtons.h"
#include "EKeyboardKeys.h"
#include "JsonValue.h"
#include "StateStack.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "RenderMessages.h"
#include "Engine.h"
#include "Renderer.h"
#include "PlayState.h"
#include "LoadState.h"
#include "ThreadedPostmaster/LoadLevelMessage.h"
#include "SplitScreenSelection.h"
#include "../Audio/AudioInterface.h"

char CMenuState::ourMenuesToPop = 0;

CMenuState::CMenuState(StateStack& aStateStack, std::string aFile) : State(aStateStack, eInputMessengerType::eMainMenu), myTextInputs(2), myCurrentTextInput(-1), myShowStateBelow(false), myPointerSprite(nullptr), myIsInFocus(false), myBlinkeyBool(true), myBlinkeyTimer(0)
{

	myManager.AddAction("ExitGame", bind(&CMenuState::ExitGame, std::placeholders::_1));
	myManager.AddAction("PushMenu", [this](std::string string)-> bool { return PushMenu(string); });
	myManager.AddAction("PopMenues", [this](std::string string)-> bool { return PopMenues(string); });
	myManager.AddAction("PushLevel", [this](std::string string)-> bool { return PushLevel(string); });
	myManager.AddAction("SelectTextInput", [this](std::string string)-> bool { return SetCurrentTextInput(string); });
	myManager.AddAction("PushSplitScreenSelection", [this](std::string string)-> bool { return PushSplitScreenSelection();});
	MenuLoad(aFile);
}

CMenuState::~CMenuState()
{
}

void CMenuState::Init()
{
}

eStateStatus CMenuState::Update(const CU::Time& aDeltaTime)
{
	myManager.Update(aDeltaTime);

	myBlinkeyTimer += aDeltaTime.GetSeconds();

	if (myBlinkeyTimer >= 1)
	{
		myBlinkeyBool = !myBlinkeyBool;
		myBlinkeyTimer = 0;
	}

	if (ourMenuesToPop > 0)
	{
		ourMenuesToPop -= 1;
		return eStateStatus::ePop;
	}
	else
	{
		return myStatus;
	}
}

void CMenuState::Render()
{
	std::wstring oldString;
	std::wstring otherOldString;
	int otherOldStringIndex = -1;

	for (int i = 0; i < myTextInputs.Size(); ++i)
	{
		if (myTextInputs[i].myInputIsValid == false)
		{
			otherOldString = myTextInputs[i].myTextInstance->GetTextLines()[0];
			otherOldStringIndex = i;
			myTextInputs[i].myTextInstance->SetTextLine(0, L"Incorect input");
		}
	}

	if (myCurrentTextInput > -1 && myBlinkeyBool == true)
	{
		CTextInstance* currentTextInstance = myTextInputs[myCurrentTextInput].myTextInstance;
		oldString = currentTextInstance->GetTextLines()[0];
		currentTextInstance->SetTextLine(0, oldString + L"I");
	}

	myManager.Render();

	if (myCurrentTextInput >= 0 && myBlinkeyBool == true)
	{
		CTextInstance* currentTextInstance = myTextInputs[myCurrentTextInput].myTextInstance;
		currentTextInstance->SetTextLine(0, oldString);
	}

	if (otherOldStringIndex > -1)
	{
		myTextInputs[otherOldStringIndex].myTextInstance->SetTextLine(0, otherOldString);
	}
}

void CMenuState::OnEnter(const bool aLetThroughRender)
{

	myManager.UpdateMousePosition(myManager.GetMopusePosition());
	Audio::CAudioInterface::GetInstance()->PostEvent("PlayMenu");
	//Audio::CAudioInterface::GetInstance()->LoadBank("Audio/KartSounds.bnk");
	//Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eCharPressed);
	//Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
	myIsInFocus = true;
}

void CMenuState::OnExit(const bool aLetThroughRender)
{
	RENDERER.ClearGui();
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
	myIsInFocus = false;
	Audio::CAudioInterface::GetInstance()->PostEvent("StopMenu");
}

CU::eInputReturn CMenuState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (myIsInFocus == false)
	{
		return CU::eInputReturn::ePassOn;
	}
	switch (aInputMessage.myType)
	{
	case CU::eInputType::eMouseMoved:
		myManager.UpdateMousePosition(myManager.GetMopusePosition() + aInputMessage.myMouseDelta);
		break;
	case CU::eInputType::eMousePressed:
		if (aInputMessage.myMouseButton == CU::eMouseButtons::LBUTTON)
		{
			myManager.MousePressed();
		}
		break;
	case CU::eInputType::eMouseReleased:
		if (aInputMessage.myMouseButton == CU::eMouseButtons::LBUTTON)
		{
			myManager.MouseReleased();
		}
		break;
	case CU::eInputType::eScrollWheelChanged: break;
	case CU::eInputType::eKeyboardPressed:
		if (aInputMessage.myKey == CU::eKeys::BACK && myCurrentTextInput > -1)
		{
			CTextInstance& currentTextInput = *myTextInputs[myCurrentTextInput].myTextInstance;
			currentTextInput.SetTextLine(0, currentTextInput.GetTextLines()[0].substr(0, currentTextInput.GetTextLines()[0].length() - 1));
		}
		else if(aInputMessage.myKey == CU::eKeys::F10)
		{
			PushLevel("0");
		}
		break;
	case CU::eInputType::eKeyboardReleased: break;
	default: break;
	}

	return CU::eInputReturn::eKeepSecret;
}

eMessageReturn CMenuState::DoEvent(const KeyCharPressed& aCharPressed)
{
	if (myCurrentTextInput < 0)
	{
		return eMessageReturn::eContinue;
	}

	CTextInstance* textInstance = myTextInputs.At(myCurrentTextInput).myTextInstance;


	//if (aCharPressed.GetKey() != 0x08 && aCharPressed.GetKey() != 0x0D)
	//{
	//	std::string blä;
	//	blä = aCharPressed.GetKey();
	//	textInstance->SetTextLine(0, textInstance->GetTextLines().At(0) + CU::StringToWString(blä));
	//}

	return eMessageReturn::eContinue;
}

eMessageReturn CMenuState::DoEvent(const CConectedMessage& aCharPressed)
{
	return eMessageReturn::eContinue;
}

eMessageReturn CMenuState::DoEvent(const CLoadLevelMessage& aLoadLevelMessage)
{
	myStateStack.PushState(new CLoadState(myStateStack, aLoadLevelMessage.myLevelIndex));
	return eMessageReturn::eContinue;

}

eAlignment CMenuState::LoadAlignment(const CU::CJsonValue& aJsonValue)
{
	if (aJsonValue.GetString() == "center")
	{
		return eAlignment::eCenter;
	}
	if (aJsonValue.GetString() == "right")
	{
		return eAlignment::eRight;
	}
	return eAlignment::eLeft;
}

void CMenuState::LoadElement(const CU::CJsonValue& aJsonValue, const std::string &aFolderpath)
{
	const std::string &name = aJsonValue.at("name").GetString();
	const CU::Vector2f position = aJsonValue.at("position").GetVector2f("xy");
	const CU::Vector2f origin = aJsonValue.at("origin").GetVector2f("xy");

	const int spriteID = myManager.CreateSprite(aFolderpath + "/" + name, position, origin, 1);

	if (aJsonValue.at("isButton").GetBool())
	{
		const CU::CJsonValue &buttonValue = aJsonValue.at("button");

		CU::CJsonValue actionArray = buttonValue.at("actions");

		CU::GrowingArray<std::string> actions(MAX(actionArray.Size(), 1));
		CU::GrowingArray<std::string> arguments(MAX(actionArray.Size(), 1));

		for (int i = 0; i < actionArray.Size(); ++i)
		{
			actions.Add(actionArray.at(i).at("type").GetString());
			arguments.Add(actionArray.at(i).at("argument").GetString());
		}

		myManager.CreateClickArea(actions, arguments, spriteID, buttonValue.at("rect").GetVector4f("xyzw"), 1);
	}

	if (aJsonValue.at("hasText").GetBool())
	{
		const CU::CJsonValue &textValue = aJsonValue.at("text");
		std::string fontName = textValue.at("font").GetString();
		std::wstring text = CU::StringToWString(textValue.at("text").GetString());

		eAlignment alignment = LoadAlignment(textValue.at("alignment"));

		CU::Vector2f textPosition;
		if (spriteID < 0)
		{
			textPosition = position + textValue.at("offset").GetVector2f();
		}
		else
		{
			const SMenuSprite& currentSprite = myManager.GetSprite(spriteID);
			textPosition = position + (textValue.at("offset").GetVector2f() - currentSprite.myDafaultSprite->GetPivot()) * currentSprite.myDafaultSprite->GetSize();
		}

		if (text.size() > 0 && text.at(0) == L'#')
		{
			std::wstring::size_type underscore = text.find(L"_");
			if (underscore == std::wstring::npos)
			{
				underscore = text.length() + 1;
			}

			std::wstring subString = text.substr(1, underscore - 1);

			if (subString == L"textInput")
			{
				const std::wstring numberString = text.substr(underscore + 1, text.size() - underscore + 1);
				const int currentTextInput = std::stoi(numberString);

				while (myTextInputs.Size() < currentTextInput + 1)
				{
					myTextInputs.Add(STextInput());
				}

				const int textInputTextInstanceIndex = myManager.CreateText(fontName, textPosition, L"", 2, alignment);
				myTextInputs[currentTextInput].myTextInstance = myManager.GetTextInstance(textInputTextInstanceIndex);
				myTextInputs[currentTextInput].myTextInstance->SetColor({ 0.f,0.f,0.f,1.f });
			}
			else if (subString == L"IP")
			{
				myManager.GetTextInstance(myManager.CreateText(fontName, textPosition, myThisComputersIP, 2, alignment))->SetColor({0.f, 0.f, 0.f , 1.f});
			}
		}
		else
		{
			myManager.CreateText(fontName, textPosition, text, 2, alignment);
		}
	}
}

void CMenuState::MenuLoad(const std::string& aFile)
{
	CU::CJsonValue root(aFile);

	myManager.SetMousePointer(new CSpriteInstance(root.at("cursor").GetString().c_str()));
	const std::string &folderPath = root.at("folder").GetString();
	myShowStateBelow = root.at("letThroughRender").GetBool();

	const CU::CJsonValue elementArray = root.at("elements");
	for (unsigned i = 0; i < elementArray.Size(); ++i)
	{
		LoadElement(elementArray.at(i), folderPath);
	}
}

bool CMenuState::PushMenu(std::string aMenu) 
{
	myStateStack.PushState(new CMenuState(myStateStack, aMenu));
	return true;
}

bool CMenuState::ExitGame(std::string /* not used*/)
{
	//Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CQuitGame);
	return true;
}

bool CMenuState::PushSplitScreenSelection()
{
	myStateStack.PushState(new CSplitScreenSelection(myStateStack));
	return true;
}

bool CMenuState::PopMenues(std::string aNumberOfMenues)
{
	ourMenuesToPop = stoi(aNumberOfMenues);
	return true;
}

bool CMenuState::PushLevel(std::string aLevelIndexString)
{
	myStateStack.PushState(new CLoadState(myStateStack, std::stoi(aLevelIndexString)));
	RENDERER.ClearGui();
	return true;
}

bool CMenuState::SetCurrentTextInput(std::string aTexINputIndex)
{
	myCurrentTextInput = stoi(aTexINputIndex);
	myTextInputs[myCurrentTextInput].myInputIsValid = true;
	return true;
}