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
#include "MenuState.h"

CSplitScreenSelection::CSplitScreenSelection(StateStack& aStateStack) : State(aStateStack, eInputMessengerType::eSplitScreenSelectionMenu, 1)
{
	myPlayers.Init(4);
	myHasKeyboardResponded = false;
	myMenuManager.AddAction("PushLevel", [this](std::string string)-> bool { return PushLevel(string); });
	myMenuManager.AddAction("BackToMenu", [this](std::string string)-> bool { return BackToMenu(string); });
	for (int i = 0; i < 4; ++i)
	{
		AddXboxController();
		SParticipant::eInputDevice input = SParticipant::eInputDevice::eNone;
		myPlayerInputDevices.Insert(i, input);
	}
	myCharacterSprites.Init(static_cast<int>(SParticipant::eCharacter::eLength));
	std::string path = "";
	for (unsigned short i = 0; i < static_cast<unsigned short>(SParticipant::eCharacter::eLength); ++i)
	{
		path = "Sprites/GUI/CharacterSelectImages/CharacterSelect_Kart_0";
		path += std::to_string(i + 1);
		path += ".dds";
		CSpriteInstance* sprite = new CSpriteInstance(path.c_str());
		sprite->SetPosition(CU::Vector2f(CU::Vector2f(0, 0)));
		myCharacterSprites.Add(sprite);
	}

	myGUIParts.Init(4);
	for (unsigned int i = 0; i < 4; ++i)
	{
		GUIPart part;
		part.hasJoined = false;
		part.JoinSprite = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/JoinImage.dds");
		part.LeftArrow = nullptr;
		part.RightArrow = nullptr;
		part.NameTag = nullptr;
		myGUIParts.Add(part);
	}
	myGUIParts[0].NameTag = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player1.dds");
	myGUIParts[0].LeftArrow = new  CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player1_arrowLeft.dds");
	myGUIParts[0].RightArrow = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player1_arrowRight.dds");
	myGUIParts[0].ReadySprite = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/IsPlayerReady.dds");
	myGUIParts[0].ReadySprite->SetPosition(CU::Vector2f::Zero);
	myGUIParts[0].JoinSprite->SetPosition(CU::Vector2f::Zero);
	myGUIParts[0].LeftArrow->SetPosition(CU::Vector2f(0.025f, 0.2f));
	myGUIParts[0].RightArrow->SetPosition(CU::Vector2f(0.45f, 0.2f));
	myGUIParts[0].NameTag->SetPosition(CU::Vector2f(0.14f, 0.4f));
	myGUIParts[0].LeftArrowOriginPosition = myGUIParts[0].LeftArrow->GetPosition();
	myGUIParts[0].RightArrowOriginPosition = myGUIParts[0].RightArrow->GetPosition();


	myGUIParts[1].LeftArrow = new  CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player2_arrowLeft.dds");
	myGUIParts[1].RightArrow = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player2_arrowRight.dds");
	myGUIParts[1].NameTag = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player2.dds");
	myGUIParts[1].ReadySprite = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/IsPlayerReady.dds");
	myGUIParts[1].ReadySprite->SetPosition(CU::Vector2f(0.5f, 0.0f));
	myGUIParts[1].JoinSprite->SetPosition(CU::Vector2f(0.5f, 0.0f));
	myGUIParts[1].LeftArrow->SetPosition(CU::Vector2f(0.525f, 0.2f));
	myGUIParts[1].RightArrow->SetPosition(CU::Vector2f(0.95f, 0.2f));
	myGUIParts[1].NameTag->SetPosition(CU::Vector2f(0.64f, 0.4f));
	myGUIParts[1].LeftArrowOriginPosition = myGUIParts[1].LeftArrow->GetPosition();
	myGUIParts[1].RightArrowOriginPosition = myGUIParts[1].RightArrow->GetPosition();

	myGUIParts[2].LeftArrow = new  CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player3_arrowLeft.dds");
	myGUIParts[2].RightArrow = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player3_arrowRight.dds");
	myGUIParts[2].NameTag = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player3.dds");
	myGUIParts[2].ReadySprite = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/IsPlayerReady.dds");
	myGUIParts[2].ReadySprite->SetPosition(CU::Vector2f(0, 0.5f));
	myGUIParts[2].JoinSprite->SetPosition(CU::Vector2f(0, 0.5f));
	myGUIParts[2].LeftArrow->SetPosition(CU::Vector2f(0.025f, 0.7f));
	myGUIParts[2].RightArrow->SetPosition(CU::Vector2f(0.45f, 0.7f));
	myGUIParts[2].NameTag->SetPosition(CU::Vector2f(0.14f, 0.9f));
	myGUIParts[2].LeftArrowOriginPosition = myGUIParts[2].LeftArrow->GetPosition();
	myGUIParts[2].RightArrowOriginPosition = myGUIParts[2].RightArrow->GetPosition();

	myGUIParts[3].LeftArrow = new  CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player4_arrowLeft.dds");
	myGUIParts[3].RightArrow = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player4_arrowRight.dds");
	myGUIParts[3].NameTag = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/Player4.dds");
	myGUIParts[3].ReadySprite = new CSpriteInstance("Sprites/GUI/CharacterSelectImages/IsPlayerReady.dds");
	myGUIParts[3].ReadySprite->SetPosition(CU::Vector2f(0.5f, 0.5f));
	myGUIParts[3].JoinSprite->SetPosition(CU::Vector2f(0.5f, 0.5f));
	myGUIParts[3].LeftArrow->SetPosition(CU::Vector2f(0.525f, 0.7f));
	myGUIParts[3].RightArrow->SetPosition(CU::Vector2f(0.95f, 0.7f));
	myGUIParts[3].NameTag->SetPosition(CU::Vector2f(0.64f, 0.9f));
	myGUIParts[3].LeftArrowOriginPosition = myGUIParts[3].LeftArrow->GetPosition();
	myGUIParts[3].RightArrowOriginPosition = myGUIParts[3].RightArrow->GetPosition();
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
	for (unsigned int i = 0; i < myGUIParts.Size(); ++i)
	{
		myGUIParts[i].Update(aDeltaTime.GetSeconds());
	}
	return eStateStatus::eKeep;
}

void CSplitScreenSelection::Render()
{
	myMenuManager.Render();
	if (myPlayers.Size() > 0)
	{
		myCharacterSprites[static_cast<short>(myPlayers[0].mySelectedCharacter)]->SetPosition(CU::Vector2f(0, 0));
		myCharacterSprites[static_cast<short>(myPlayers[0].mySelectedCharacter)]->RenderToGUI(L"__Menu");
		if (myPlayers.Size() > 1)
		{
			myCharacterSprites[static_cast<short>(myPlayers[1].mySelectedCharacter)]->SetPosition(CU::Vector2f(0.5f, 0));
			myCharacterSprites[static_cast<short>(myPlayers[1].mySelectedCharacter)]->RenderToGUI(L"__Menu");
			if (myPlayers.Size() > 2)
			{
				myCharacterSprites[static_cast<short>(myPlayers[2].mySelectedCharacter)]->SetPosition(CU::Vector2f(0, 0.5f));
				myCharacterSprites[static_cast<short>(myPlayers[2].mySelectedCharacter)]->RenderToGUI(L"__Menu");
				if (myPlayers.Size() > 3)
				{
					myCharacterSprites[static_cast<short>(myPlayers[3].mySelectedCharacter)]->SetPosition(CU::Vector2f(0.5f, 0.5f));
					myCharacterSprites[static_cast<short>(myPlayers[3].mySelectedCharacter)]->RenderToGUI(L"__Menu");
				}
			}
		}
	}
	for (unsigned int i = 0; i < myGUIParts.Size(); ++i)
	{
		myGUIParts[i].Redner();
	}
	for (unsigned int i = 0; i < myPlayers.Size(); ++i)
	{
		if (myPlayers[i].myIsReady == true)
		{
			myGUIParts[i].ReadySprite->RenderToGUI(L"__Menu");
		}
	}
}

void CSplitScreenSelection::OnEnter(const bool aLetThroughRender)
{
	myMenuManager.UpdateMousePosition(myMenuManager.GetMopusePosition());
}

void CSplitScreenSelection::OnExit(const bool aLetThroughRender)
{
	for (unsigned int i = 0; i < myGUIParts.Size(); ++i)
	{
		myGUIParts[i].Delete();
	}
	myCharacterSprites.DeleteAll();
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

		/*CU::Vector2f textPosition;
		if (spriteID < 0)
		{
			textPosition = position + textValue.at("offset").GetVector2f();
		}
		else
		{
			const SMenuSprite& currentSprite = myMenuManager.GetSprite(spriteID);
			textPosition = position + (textValue.at("offset").GetVector2f() - currentSprite.mySprites[0]->GetPivot()) * currentSprite.mySprites[0]->GetSize();
		}*/

		/*if (text.size() > 0 && text.at(0) == L'#')
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
		}*/
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

void CSplitScreenSelection::RightChar(SParticipant& aParticipant)
{
	for (unsigned int i = 0; i < myPlayers.Size(); ++i)
	{
		if (myPlayers[i] == aParticipant)
		{
			int index = i;
			myGUIParts[index].RightArrow->SetPosition(CU::Vector2f(myGUIParts[index].RightArrowOriginPosition.x + 0.025f, myGUIParts[index].RightArrow->GetPosition().y));
			break;
		}
	}
	short u = static_cast<short>(aParticipant.mySelectedCharacter);
	++u;
	if (u >= static_cast<short>(SParticipant::eCharacter::eLength))
	{
		u = 0;
	}
	aParticipant.mySelectedCharacter = static_cast<SParticipant::eCharacter>(u);

}

void CSplitScreenSelection::LeftChar(SParticipant& aParticipant)
{
	for (unsigned int i = 0; i < myPlayers.Size(); ++i)
	{
		if (myPlayers[i] == aParticipant)
		{
			int index = i;
			myGUIParts[index].LeftArrow->SetPosition(CU::Vector2f(myGUIParts[index].LeftArrowOriginPosition.x - 0.025f, myGUIParts[index].LeftArrow->GetPosition().y));
			break;
		}
	}
	short u = static_cast<short>(aParticipant.mySelectedCharacter);
	--u;
	if (u < 0)
	{
		u = static_cast<short>(SParticipant::eCharacter::eLength) - 1;
	}
	aParticipant.mySelectedCharacter = static_cast<SParticipant::eCharacter>(u);
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
					participant.myIsReady = false;
					myPlayers.Add(participant);
					myPlayerInputDevices[myPlayers.Size() - 1] = static_cast<SParticipant::eInputDevice>(aInputMessage.myGamepadIndex);
					myGUIParts[myPlayers.Size() - 1].hasJoined = true;
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
				myMenuManager.ourParticipants = myPlayers;
				myStateStack.SwapState(new CMenuState(myStateStack, "Json/Menu/ControllerLevelSelect.json"));
			}
			break;
		case CU::GAMEPAD::DPAD_RIGHT:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
				{
					RightChar(myPlayers[i]);
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
					LeftChar(myPlayers[i]);
				}
			}
		}
		break;
		}
	}
	else if (aInputMessage.myType == CU::eInputType::eGamePadLeftJoyStickChanged)
	{
		float inbuiltdeadzone = 0.7f;
		if ((myLastJoyX < inbuiltdeadzone && aInputMessage.myJoyStickPosition.x > inbuiltdeadzone) || (myLastJoyX > -inbuiltdeadzone && aInputMessage.myJoyStickPosition.x < -inbuiltdeadzone))
		{
			if (aInputMessage.myJoyStickPosition.x > inbuiltdeadzone)
			{
				for (unsigned int i = 0; i < myPlayers.Size(); ++i)
				{
					if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
					{
						RightChar(myPlayers[i]);
					}
				}
			}
			else if (aInputMessage.myJoyStickPosition.x < -inbuiltdeadzone)
			{
				for (unsigned int i = 0; i < myPlayers.Size(); ++i)
				{
					if (static_cast<short>(myPlayers[i].myInputDevice) == aInputMessage.myGamepadIndex)
					{
						LeftChar(myPlayers[i]);
					}
				}
			}
		}
		myLastJoyX = aInputMessage.myJoyStickPosition.x;

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
					participant.myIsReady = false;
					myPlayers.Add(participant);
					myPlayerInputDevices[myPlayers.Size() - 1] = static_cast<SParticipant::eInputDevice>(aInputMessage.myGamepadIndex);
					myGUIParts[myPlayers.Size() - 1].hasJoined = true;
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
		case CU::eKeys::D:
		case CU::eKeys::RIGHT:
		{
			bool found = false;
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (myPlayers[i].myInputDevice == SParticipant::eInputDevice::eKeyboard)
				{
					RightChar(myPlayers[i]);
				}
			}
		}
		break;
		case CU::eKeys::A:
		case CU::eKeys::LEFT:
		{
			for (unsigned int i = 0; i < myPlayers.Size(); ++i)
			{
				if (myPlayers[i].myInputDevice == SParticipant::eInputDevice::eKeyboard)
				{
					LeftChar(myPlayers[i]);
				}
			}
		}
		break;
		case CU::eKeys::RETURN:
			if (myPlayers.Size() >= 1)
			{
				myMenuManager.ourParticipants = myPlayers;
				myStateStack.SwapState(new CMenuState(myStateStack, "Json/Menu/ControllerLevelSelect.json"));
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
