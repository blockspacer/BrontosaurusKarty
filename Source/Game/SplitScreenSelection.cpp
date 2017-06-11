#include "stdafx.h"
#include "SplitScreenSelection.h"
#include "ThreadedPostmaster/Postmaster.h"
#include "CommonUtilities/EInputReturn.h"
#include "CommonUtilities/InputMessage.h"
#include "CommonUtilities/XInputWrapper.h"
#include "CommonUtilities/EKeyboardKeys.h"
#include "CommonUtilities/JsonValue.h"
#include "StateStack.h"
#include "LoadState.h"
#include "CommonUtilities.h"

CSplitScreenSelection::CSplitScreenSelection(StateStack& aStateStack) : State(aStateStack,eInputMessengerType::eSplitScreenSelectionMenu, 1)
{
	myPlayers.Init(4);
	myHasKeyboardResponded = false;
	//myStateStack = &aStateStack;

	//borde ändras till något annat senare men eh
	myMenuManager.AddAction("PushLevel", [this](std::string string)-> bool { return PushLevel(string); });
	myMenuManager.AddAction("BackToMenu", [this](std::string string)-> bool { return BackToMenu(string); });
	MenuLoad("Json/Menu/SplitScreenSelection.json");
	for (int i = 0; i < 4; ++i)
	{
		AddXboxController();
		SParticipant::eInputDevice input = SParticipant::eInputDevice::eNone;
		myPlayerInputDevices.Insert(i, input);
	}
	
}


CSplitScreenSelection::~CSplitScreenSelection()
{
}

void CSplitScreenSelection::Init()
{
}

eStateStatus CSplitScreenSelection::Update(const CU::Time & aDeltaTime)
{
	myMenuManager.Update(aDeltaTime);
	return eStateStatus::eKeep;
}

void CSplitScreenSelection::Render()
{
	myMenuManager.Render();
}

void CSplitScreenSelection::OnEnter(const bool aLetThroughRender)
{
	myMenuManager.UpdateMousePosition(myMenuManager.GetMopusePosition());
}

void CSplitScreenSelection::OnExit(const bool aLetThroughRender)
{

}

void CSplitScreenSelection::MenuLoad(const std::string & aFile)
{
	CU::CJsonValue root;
	root.Parse(aFile);

	myMenuManager.SetMousePointer(new CSpriteInstance(root.at("cursor").GetString().c_str()));
	const std::string &folderPath = root.at("folder").GetString();
	myShowStateBelow = root.at("letThroughRender").GetBool();

	const CU::CJsonValue elementArray = root.at("elements");
	for (unsigned i = 0; i < elementArray.Size(); ++i)
	{
		LoadElement(elementArray.at(i), folderPath);
	}
}

void CSplitScreenSelection::LoadElement(const CU::CJsonValue & aJsonValue, const std::string & aFolderpath)
{
	const std::string &name = aJsonValue.at("name").GetString();
	const CU::Vector2f position = aJsonValue.at("position").GetVector2f("xy");
	const CU::Vector2f origin = aJsonValue.at("origin").GetVector2f("xy");
	int playerIndexThatiListenTo = -1;
	if (name[0] == '#')
	{
		//make controller controlled gui
		playerIndexThatiListenTo = aJsonValue.at("listeningToPlayer").GetInt();
	}

	const int spriteID = myMenuManager.CreateSprite(aFolderpath + "/" + name, position, origin, 1, playerIndexThatiListenTo);

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

		myMenuManager.CreateClickArea(actions, arguments, spriteID, buttonValue.at("rect").GetVector4f("xyzw"), 1);
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
			const SMenuSprite& currentSprite = myMenuManager.GetSprite(spriteID);
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

				const int textInputTextInstanceIndex = myMenuManager.CreateText(fontName, textPosition, L"", 2, alignment);
				myTextInputs[currentTextInput].myTextInstance = myMenuManager.GetTextInstance(textInputTextInstanceIndex);
				myTextInputs[currentTextInput].myTextInstance->SetColor({ 0.f,0.f,0.f,1.f });
			}
		}
		else
		{
			myMenuManager.CreateText(fontName, textPosition, text, 2, alignment);
		}
	}
}

eAlignment CSplitScreenSelection::LoadAlignment(const CU::CJsonValue & aJsonValue)
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

bool CSplitScreenSelection::PushLevel(const std::string & aString)
{
	return false;
}

bool CSplitScreenSelection::BackToMenu(const std::string & aString)
{
	myStateStack.Pop();
	return false;
}


CU::eInputReturn CSplitScreenSelection::RecieveInput(const CU::SInputMessage & aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eGamePadButtonPressed)
	{
		switch (aInputMessage.myGamePad)
		{
		case CU::GAMEPAD::A:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
				{
					found = true;
				}
			}
			if (found == false)
			{
				if (myPlayers.Size() < 4)
				{
					SParticipant participant;
					participant.myInputDevice = static_cast<SParticipant::eInputDevice>(aInputMessage.myGamepadIndex);
					participant.mySelectedCharacter = SParticipant::eCharacter::eVanBrat;
					myPlayers.Add(participant);
					myPlayerInputDevices[myPlayers.Size() - 1] = static_cast<SParticipant::eInputDevice>(aInputMessage.myGamepadIndex);
				}
			}
		}
			break;
		case CU::GAMEPAD::B:
			myStateStack.Pop();
			break;
		case CU::GAMEPAD::START:
			if (myPlayers.Size() >= 1)
			{
				myStateStack.SwapState(new CLoadState(myStateStack,0, myPlayers));
			}
			break;
		case CU::GAMEPAD::DPAD_UP:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eVanBrat;
				}
			}
		}
		break;
		case CU::GAMEPAD::DPAD_RIGHT:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eVanBrat2;
				}
			}
		}
		break;
		case CU::GAMEPAD::DPAD_DOWN:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eGrandMa;
				}
			}
		}
		break;
		case CU::GAMEPAD::DPAD_LEFT:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eGrandMa2;
				}
			}
		}
		break;
		}
	}
	else if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		switch (aInputMessage.myKey)
		{
		case CU::eKeys::SPACE:
			if (myHasKeyboardResponded == false)
			{
				if (myPlayers.Size() < 4)
				{
					myHasKeyboardResponded = true;
					SParticipant participant;
					participant.mySelectedCharacter = SParticipant::eCharacter::eVanBrat;
					participant.myInputDevice = SParticipant::eInputDevice::eKeyboard;
					myPlayers.Add(participant);
					myPlayerInputDevices[myPlayers.Size() - 1] = static_cast<SParticipant::eInputDevice>(aInputMessage.myGamepadIndex);
				}
			}
			break;		
		case CU::eKeys::ESCAPE:
			if (myHasKeyboardResponded == true)
			{
				for (unsigned int i = 0; i < myPlayers.Size(); ++i)
				{
					if (myPlayers[i].myInputDevice == SParticipant::eInputDevice::eKeyboard)
					{
						myHasKeyboardResponded = false;
						myPlayerInputDevices[i] = SParticipant::eInputDevice::eNone;
						myPlayers.RemoveAtIndex(i);
					}
				}
			}
			break;
			if (myHasKeyboardResponded == false)
			{
				myHasKeyboardResponded = true;
				SParticipant participant;
				participant.myInputDevice = SParticipant::eInputDevice::eKeyboard;
			}
			break;
		case CU::eKeys::UP:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (myPlayers[i].myInputDevice == SParticipant::eInputDevice::eKeyboard)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eVanBrat;
				}
			}
		}
		break;
		case CU::eKeys::RIGHT:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (myPlayers[i].myInputDevice == SParticipant::eInputDevice::eKeyboard)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eVanBrat2;
				}
			}
		}
		break;
		case CU::eKeys::DOWN:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (myPlayers[i].myInputDevice == SParticipant::eInputDevice::eKeyboard)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eGrandMa;
				}
			}
		}
		break;
		case CU::eKeys::LEFT:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (myPlayers[i].myInputDevice == SParticipant::eInputDevice::eKeyboard)
				{
					myPlayers[i].mySelectedCharacter = SParticipant::eCharacter::eGrandMa2;
				}
			}
		}
		case CU::eKeys::RETURN:
			if (myPlayers.Size() >= 1)
			{
				myStateStack.SwapState(new CLoadState(myStateStack, 0, myPlayers));
			}
				break;
		default:
			break;
		}
	}
	else if (aInputMessage.myType == CU::eInputType::eMouseMoved)
	{
		myMenuManager.UpdateMousePosition(myMenuManager.GetMopusePosition() + aInputMessage.myMouseDelta);
	}
	else if (aInputMessage.myType == CU::eInputType::eMousePressed)
	{
		myMenuManager.MousePressed();
	}
	else if (aInputMessage.myType == CU::eInputType::eMouseReleased)
	{
		myMenuManager.MouseReleased();
	}

	for (unsigned int i = 0; i < myPlayerInputDevices.Size(); ++i)
	{
		if (myPlayerInputDevices[i] == static_cast<SParticipant::eInputDevice>(aInputMessage.myGamepadIndex))
		{
			if (aInputMessage.myType == CU::eInputType::eGamePadButtonPressed)
			{
				switch (aInputMessage.myGamePad)
				{
				case CU::GAMEPAD::A:
					myMenuManager.ActionPressed(static_cast<short>(i));
					break;
				case CU::GAMEPAD::DPAD_LEFT:
					myMenuManager.LeftPressed(static_cast<short>(i));
					break;
				case CU::GAMEPAD::DPAD_RIGHT:
					myMenuManager.RightPressed(static_cast<short>(i));
					break;
				default:
					break;
				}
			}
			else if (aInputMessage.myType == CU::eInputType::eGamePadLeftJoyStickChanged)
			{
				if (aInputMessage.myJoyStickPosition.x > 0.5f)
				{
					myMenuManager.LeftPressed(static_cast<short>(myPlayerInputDevices[i]));
				}
				else if (aInputMessage.myJoyStickPosition.x < 0.5f)
				{
					myMenuManager.RightPressed(static_cast<short>(myPlayerInputDevices[i]));
				}
			}
			else if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
			{
				switch (aInputMessage.myKey)
				{
				case CU::eKeys::A:
				case CU::eKeys::LEFT:
					myMenuManager.LeftPressed(static_cast<short>(i));
					break;
				case CU::eKeys::D:
				case CU::eKeys::RIGHT:
					myMenuManager.RightPressed(static_cast<short>(i));
					break;
				case CU::eKeys::SPACE:
					myMenuManager.ActionPressed(static_cast<short>(i));
					break;
				}
			}
		}
	}
	return CU::eInputReturn::eKeepSecret;
}
