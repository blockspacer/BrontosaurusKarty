#include "stdafx.h"
#include "MenuManager.h"
#include "../BrontosaurusEngine/RenderMessages.h"
#include "../BrontosaurusEngine/Renderer.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../CommonUtilities/JsonValue.h"


CU::Vector2f CMenuManager::ourMousePosition(0.5f, 0.5f);

bool CompareLayers(SLayerData aFirstData, SLayerData aSecondData)
{
	return aFirstData.myLayer > aSecondData.myLayer;
}

CMenuManager::CMenuManager() : myPointerSprite(nullptr), myShouldRender(true), myCurentlyHoveredClickarea(-1), myInput(eInputAction::eNone), myMouseIsPressed(false),myUsedGamepadButtons(4),myGamepadAction(4)
{
	myClickAreas.Init(1);
	mySpriteInstances.Init(1);
	myTextInstances.Init(1);
	myLayers.Init(1);

	myPlayerThatpressed = -1;
	myIsLeftPressed = false;
	myIsRightPressed = false;

	myGUIElement.myOrigin = CU::Vector2f(0.5f, 0.5f);
	myGUIElement.myScreenRect = CU::Vector4f(0.5f, 0.5f, 1.5f, 1.5f);
	myHasPlayedHoverSound = false;
	myHasPlayedClickSound = false;
}


CMenuManager::~CMenuManager()
{
}

void CMenuManager::CreateClickArea(CU::GrowingArray<std::string> someActions, CU::GrowingArray<std::string> someArguments, const int aSpriteID, CU::Vector4f aRect, const unsigned char aLayer)
{
	SClickArea clickArea;
	clickArea.myRect = aRect;
	clickArea.mySpriteID = aSpriteID;
	clickArea.myActions.Init(1);

	for (int i = 0; i < someActions.Size(); ++i)
	{
		clickArea.myActions.Add(bind(myActions[someActions[i]], someArguments[i]));
	}

	myClickAreas.Add(clickArea);

	myLayers.Add({ aLayer, myClickAreas.Size() - 1, eMenuThingType::eClickArea });
}

void CMenuManager::AddGamepadAction(const CU::GAMEPAD aGamepadButton,const CU::GrowingArray<std::string>& someActions,const CU::GrowingArray<std::string>& someArguments)
{
	int buttonIndex = myUsedGamepadButtons.Find(aGamepadButton);
	if (buttonIndex == myUsedGamepadButtons.FoundNone)
	{
		buttonIndex = myUsedGamepadButtons.Size();
		myUsedGamepadButtons.Add(aGamepadButton);
		myGamepadAction.Add(CU::GrowingArray<std::function<bool()>>(someActions.Size()));
	}

	CU::GrowingArray<std::function<bool()>>& actions = myGamepadAction[buttonIndex];

	for (unsigned i = 0; i < someActions.Size(); ++i)
	{
		actions.Add(bind(myActions[someActions[i]], someArguments[i]));
	}
}

int CMenuManager::CreateSprite(const std::string& aFolder, const CU::Vector2f aPosition, const CU::Vector2f anOrigin, const unsigned char aLayer, const short aListeningToPlayer)
{
	SMenuSprite menuSprite;

	int index = 0;
	while (CU::CJsonValue::FileExists(aFolder + "/" + std::to_string(index) + ".dds"))
	{
		menuSprite.mySprites.Add(new CSpriteInstance((aFolder + "/" + std::to_string(index) + ".dds").c_str()));
		menuSprite.mySprites[index]->SetPosition(aPosition);
		menuSprite.mySprites[index]->SetPivot(anOrigin);
		++index;
	}

	if(index == 0)
	{
		menuSprite.mySprites = CU::GrowingArray<CSpriteInstance*, char>(4, nullptr);

		if (CU::CJsonValue::FileExists(aFolder + "/" + "default.dds"))
		{
			menuSprite.mySprites[0] = new CSpriteInstance((aFolder + "/" + "default.dds").c_str());
			menuSprite.mySprites[0]->SetPosition(aPosition);
			menuSprite.mySprites[0]->SetPivot(anOrigin);
		}

		if (CU::CJsonValue::FileExists(aFolder + "/" + "onHover.dds"))
		{
			menuSprite.mySprites[1] = new CSpriteInstance((aFolder + "/" + "onHover.dds").c_str());
			menuSprite.mySprites[1]->SetPosition(aPosition);
			menuSprite.mySprites[1]->SetPivot(anOrigin);
		}
		else
		{
			DL_PRINT("on hover sprite missing using default instead");
		}

		if (CU::CJsonValue::FileExists(aFolder + "/" + "onClick.dds"))
		{
			menuSprite.mySprites[2] = new CSpriteInstance((aFolder + "/" + "onClick.dds").c_str());
			menuSprite.mySprites[2]->SetPosition(aPosition);
			menuSprite.mySprites[2]->SetPivot(anOrigin);
		}
		else
		{
			DL_PRINT("on click sprite missing using default instead");
		}

		if (CU::CJsonValue::FileExists(aFolder + "/" + "inactive.dds"))
		{
			menuSprite.mySprites[3] = new CSpriteInstance((aFolder + "/" + "inactive.dds").c_str());
			menuSprite.mySprites[3]->SetPosition(aPosition);
			menuSprite.mySprites[3]->SetPivot(anOrigin);
		}
		else
		{
			DL_PRINT("inactive sprite missing using default instead");
		}
	}

	if (aListeningToPlayer != -1)
	{
		DL_PRINT("Creating gui element controlled by a player");
	}
	menuSprite.myPlayerIndex = aListeningToPlayer;
	mySpriteInstances.Add(menuSprite);

	myLayers.Add({ aLayer, mySpriteInstances.Size() - 1, eMenuThingType::eSprite });

	return mySpriteInstances.Size() - 1;
}

unsigned CMenuManager::CreateText(const std::string& aFontName, const CU::Vector2f& aPosition, const std::wstring someText, const unsigned char aLayer, const eAlignment anAlignment)
{
	CTextInstance* newText = new CTextInstance();
	newText->Init(aFontName);
	newText->SetPosition(aPosition);
	newText->SetText(someText);
	newText->SetAlignment(anAlignment);

	myTextInstances.Add(newText);

	myShouldRender = true;
	myLayers.Add({ aLayer, myTextInstances.Size() - 1, eMenuThingType::eText });
	return myTextInstances.Size() - 1;
}

void CMenuManager::SetMousePointer(CSpriteInstance* aMousePointer)
{
	if (aMousePointer != nullptr)
	{
		delete myPointerSprite;
		myPointerSprite = nullptr;
	}

	myPointerSprite = aMousePointer;
	myPointerSprite->SetPosition(ourMousePosition);
}

void CMenuManager::Update(const CU::Time& aDeltaTime)
{
	bool hasCollided = false;
	for (unsigned i = 0; i < myClickAreas.Size(); ++i)
	{
		CU::Vector2f lowerRight;
		CU::Vector2f upperLeft;

		if (myClickAreas.At(i).mySpriteID >= 0)
		{
			CSpriteInstance* currentSprite = mySpriteInstances[myClickAreas[i].mySpriteID].mySprites[0];
			lowerRight = currentSprite->GetPosition() + currentSprite->GetSize() * myClickAreas[i].myRect.zw - currentSprite->GetPivot() * currentSprite->GetSize();
			upperLeft = currentSprite->GetPosition() + currentSprite->GetSize() * myClickAreas[i].myRect.xy - currentSprite->GetPivot() * currentSprite->GetSize();
		}
		else
		{
			upperLeft = myClickAreas[i].myRect.xy;
			lowerRight = myClickAreas[i].myRect.zw;
		}

		if (ourMousePosition.x > upperLeft.x && ourMousePosition.y > upperLeft.y && ourMousePosition.x < lowerRight.x && ourMousePosition.y < lowerRight.y)
		{
			if (myCurentlyHoveredClickarea != i)
			{
				if (myCurentlyHoveredClickarea > -1 && myClickAreas[myCurentlyHoveredClickarea].mySpriteID >= 0)
				{
					mySpriteInstances[myClickAreas[myCurentlyHoveredClickarea].mySpriteID].myState = static_cast<char>(eMenuButtonState::eDefault);
				}
				myCurentlyHoveredClickarea = i;
			}

			if (myMouseIsPressed == true && myClickAreas[myCurentlyHoveredClickarea].mySpriteID >= 0 && mySpriteInstances[myClickAreas[i].mySpriteID].myState != static_cast<char>(eMenuButtonState::eOnClick))
			{
				if (myHasPlayedClickSound == false)
				{
					//Audio::CAudioInterface::GetInstance()->PostEvent("Menu_Click");
					//myHasPlayedClickSound = true;
				}
			
				mySpriteInstances[myClickAreas[i].mySpriteID].myState = static_cast<char>(eMenuButtonState::eOnClick);
			}
			else if (myClickAreas[i].mySpriteID >= 0)
			{
				mySpriteInstances[myClickAreas[i].mySpriteID].myState = static_cast<char>(eMenuButtonState::eOnHover);
			}
			hasCollided = true;
			break;
		}
	}



	if (hasCollided == false && myCurentlyHoveredClickarea > -1)
	{
		if (myClickAreas[myCurentlyHoveredClickarea].mySpriteID >= 0)
		{
			mySpriteInstances[myClickAreas[myCurentlyHoveredClickarea].mySpriteID].myState = static_cast<char>(eMenuButtonState::eDefault);
			myHasPlayedHoverSound = false;
			myHasPlayedClickSound = false;
		}
		myCurentlyHoveredClickarea = -1;
	}

	if (hasCollided == true && myHasPlayedHoverSound == false)
	{
		//Audio::CAudioInterface::GetInstance()->("Menu_Hover");
		//myHasPlayedHoverSound = true;
	}
}

void CMenuManager::Render()
{
	if (myShouldRender == true)
	{
		std::wstring string = L"__Menu";
		SCreateOrClearGuiElement* elementMessage = new SCreateOrClearGuiElement(string, myGUIElement, CU::Vector2ui(1920, 1080));
		RENDERER.AddRenderMessage(elementMessage);

		SChangeStatesMessage* const changeStatesMessage = new SChangeStatesMessage();
		changeStatesMessage->myBlendState = eBlendState::eAlphaBlend;
		changeStatesMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStatesMessage->myRasterizerState = eRasterizerState::eNoCulling;
		changeStatesMessage->mySamplerState = eSamplerState::eClamp;

		SRenderToGUI*const guiChangeState = new SRenderToGUI(L"__Menu", changeStatesMessage);
		RENDERER.AddRenderMessage(guiChangeState);

		myLayers.QuickSort(&CompareLayers);

		for (unsigned i = 0; i < myLayers.Size(); ++i)
		{
			SLayerData &layeredThing = myLayers.At(i);

			switch (layeredThing.myMenuThing)
			{
			case eMenuThingType::eText:
			{
				CTextInstance * currentTextInstance = myTextInstances.At(layeredThing.myIndex);
				currentTextInstance->RenderToGUI(L"__Menu");
			}
			break;
			case eMenuThingType::eSprite:
			{
				CSpriteInstance *spriteInstance = ChoseSpriteInstance(mySpriteInstances.At(layeredThing.myIndex));

				if (spriteInstance == nullptr)
				{
					spriteInstance = mySpriteInstances.At(layeredThing.myIndex).mySprites[0];
				}

				if (spriteInstance != nullptr)
				{
					spriteInstance->RenderToGUI(L"__Menu");
				}
			}
			break;
			case eMenuThingType::eClickArea:
			default: break;
			}
		}

		if (myPointerSprite != nullptr)
		{
			myPointerSprite->RenderToGUI(L"__Menu");
		}
	}


}

void CMenuManager::MousePressed()
{
	myMouseIsPressed = true;
}

void CMenuManager::MouseReleased()
{
	myMouseIsPressed = false;

	if (myCurentlyHoveredClickarea >= 0)
	{
		for (int i = 0; i < myClickAreas[myCurentlyHoveredClickarea].myActions.Size(); ++i)
		{
			if (myClickAreas[myCurentlyHoveredClickarea].myActions[i]() == false)
			{
				break;
			}
		}
	}
}

void CMenuManager::LeftPressed(const short aPlayerIndex)
{
	myInput = eInputAction::eLeftPressed;
	myPlayerThatpressed = aPlayerIndex;
}

void CMenuManager::RightPressed(const short aPlayerIndex)
{
	myInput = eInputAction::eRightPressed;
	myPlayerThatpressed = aPlayerIndex;
}

void CMenuManager::LeftReleased(const short aPlayerIndex)
{
}

void CMenuManager::RightReleased(const short aPlayerIndex)
{
}

void CMenuManager::ActionPressed(const short aPlayerIndex)
{
	myInput = eInputAction::eActionPressed;
	myPlayerThatpressed = aPlayerIndex;
	for (unsigned int i  = 0; i < mySpriteInstances.Size(); ++i)
	{
		if (mySpriteInstances[i].myPlayerIndex == aPlayerIndex)
		{
			mySpriteInstances[i].myState = static_cast<char>(eMenuButtonState::eOnHover);
		}
	}
}

void CMenuManager::BackButtonPressed(const short aPlayerIndex)
{
	myInput = eInputAction::eBackPressed;
	myPlayerThatpressed = aPlayerIndex;
}

void CMenuManager::RecieveGamePadInput(const CU::GAMEPAD aGamepadInput)
{
	const int actionIndex = myUsedGamepadButtons.Find(aGamepadInput);

	if (actionIndex != myUsedGamepadButtons.FoundNone)
	{
		for (unsigned i = 0; i < myGamepadAction[actionIndex].Size(); ++i)
		{
			if (myGamepadAction[actionIndex][i]() == false)
			{
				break;
			}
		}
	}
}

const SMenuSprite& CMenuManager::GetSprite(unsigned aSpriteId)
{
	return mySpriteInstances.At(aSpriteId);
}

void CMenuManager::AddAction(const std::string &aActionName, const std::function<bool(std::string)> &aFunction)
{
	myActions[aActionName] = aFunction;
}

CTextInstance* CMenuManager::GetTextInstance(const int aTextInputTextInstanceIndex)
{
	return myTextInstances[aTextInputTextInstanceIndex];
}

void CMenuManager::SetSpiteState(const unsigned aSpriteIndex, const char aState)
{
	mySpriteInstances[aSpriteIndex].myState = aState;
}

char CMenuManager::GetSpriteAmount(const int aSpriteId) const
{
	return mySpriteInstances[aSpriteId].mySprites.Size();
}

CSpriteInstance* CMenuManager::ChoseSpriteInstance(const SMenuSprite& aMenuSprite)
{
	return aMenuSprite.mySprites[aMenuSprite.myState];
}
