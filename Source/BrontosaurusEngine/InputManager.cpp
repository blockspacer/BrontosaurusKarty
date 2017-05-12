#include "stdafx.h"
#include "InputManager.h"

#include "Engine.h"
#include "WindowsWindow.h"

#include "../CommonUtilities/InputWrapper.h"
#include "../CommonUtilities/XInputWrapper.h"
#include "../CommonUtilities/EKeyboardKeys.h"
#include "../CommonUtilities/EMouseButtons.h"
#include "../CommonUtilities/EInputMessage.h"

#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"

#include "../CommonUtilities/InputMessenger.h"
#include "../CommonUtilities/InputMessage.h"
#include "../CommonUtilities/EInputReturn.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../PostMaster/FocusChange.h"
#include "../PostMaster/SetVibrationOnController.h"
#include "../PostMaster/StopVibrationOnController.h"

CInputManager* CInputManager::ourInstance = nullptr;

CInputManager::CInputManager()
	: myKeys(20)
	, myMessengers(8u)
	, myPadInputs(14)
	, myDInputWrapper(nullptr)
	, myXInputWrapper(nullptr)
{
	assert(ourInstance == nullptr && "Should not create two input managers");
	ourInstance = this;

	myDInputWrapper = new CU::InputWrapper();

	HINSTANCE hingsten = CEngine::GetInstance()->GetWindow()->GetHinstance();
	HWND hunden = CEngine::GetInstance()->GetWindow()->GetHWND();

	/*bool directInputSuccess =*/ myDInputWrapper->Init(hingsten, hunden);

	myXInputWrapper = new CU::XInputWrapper();
	//myXInputWrapper->Init(1);

	myControllerVibrationStates.Init(4);
	for (unsigned int i = 0; i < 4; ++i)
	{
		SControllerVibrationState state;
		state.myController = i;
		state.myLeftIntensity = 0;
		state.myRightIntensity = 0;
		state.myTimeToRumble = 0;
		state.myTimeIHaveRumbled = 2.5f;
		myControllerVibrationStates.Add(state);
	}

	myDInputWrapper->Init(hingsten, hunden);

	CU::Vector2f windowSize(WINDOW_SIZE);
	CU::Vector2f middleOfWindow = windowSize / 2.f;
	myLastMousePosition = middleOfWindow;

	myHasFocus = true;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eFocusChanged);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eSetVibration);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eStopVibration);
}

CInputManager::~CInputManager()
{
	assert(ourInstance != nullptr && "Missed setting ourInstance to this in constructor?");
	assert(ourInstance == this && "Should not create two input managers");
	ourInstance = nullptr;

	SAFE_DELETE(myDInputWrapper);
	SAFE_DELETE(myXInputWrapper);

	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void CInputManager::SetMousePosition(const CU::Vector2f& aMousePosition)
{
	myDInputWrapper->SetMousePosition(static_cast<int>(aMousePosition.x), static_cast<int>(aMousePosition.y));
}

void CInputManager::LockUnlockMouse(const bool aHasFocus)
{
	myHasFocus = aHasFocus;
	ShowCursor(!aHasFocus);
}

void CInputManager::Listen(CU::CInputMessenger& aMessenger, const int aPriority)
{
	if (aPriority > 0)
	{
		for (unsigned int i = 0; i < myMessengers.Size(); ++i)
		{
			if (myMessengers[i]->GetPriority() < aPriority)
			{
				myMessengers.Insert(i, &aMessenger);
				return;
			}
		}
	}

	myMessengers.Add(&aMessenger);
}

void CInputManager::Neglect(CU::CInputMessenger& aMessenger)
{
	myMessengers.Remove(&aMessenger);
}

int CInputManager::AddXboxController()
{
	return myXInputWrapper->AddController();
}

CInputManager* CInputManager::GetInstance()
{
	return ourInstance;
}

eMessageReturn CInputManager::DoEvent(const FocusChange& aFocusChange)
{
	LockUnlockMouse(aFocusChange.GetHasFocus());
	return eMessageReturn::eContinue;
}

eMessageReturn CInputManager::DoEvent(const SetVibrationOnController& aSetVibrationmessage)
{
	SControllerVibrationState* state = &myControllerVibrationStates[aSetVibrationmessage.GetJoystickIndex()];
	state->myRightIntensity = aSetVibrationmessage.GetRightIntensity();
	state->myTimeToRumble = aSetVibrationmessage.GetLeftIntensity();
	state->myTimeToRumble = aSetVibrationmessage.GetMyTimeToRumble();
	state->myShouldRumbleForever = aSetVibrationmessage.GetShoudlRumbleForever();
	return eMessageReturn::eContinue;
}

eMessageReturn CInputManager::DoEvent(const StopVibrationOnController& aStopVibrationmessage)
{
	myXInputWrapper->StopVibration(aStopVibrationmessage.GetJoyStick());
	SControllerVibrationState* state = &myControllerVibrationStates[aStopVibrationmessage.GetJoyStick()];
	state->myShouldRumbleForever = false;
	state->myLeftIntensity = 0;
	state->myRightIntensity = 0;
	state->myTimeToRumble = 0;
	state->myTimeIHaveRumbled = 2.5f;

	return eMessageReturn::eContinue;
}

void CInputManager::Update(const CU::Time& aDeltaTime)
{
	myDInputWrapper->Update();
	myXInputWrapper->UpdateStates();

	UpdateMouse();
	UpdateKeyboard();
	UpdateGamePad(aDeltaTime);
}

void CInputManager::UpdateMouse()
{
	if (myHasFocus == true)
	{
		CU::Vector2i windowSize(WINDOW_SIZE);

		static CU::Vector2i windowOffset;
		static bool locGetRect = true;
		if (locGetRect)
		{
			locGetRect = false;
			RECT rect;
			GetWindowRect(GetFocus(), &rect);
			windowOffset.x = rect.left;
			windowOffset.y = rect.top;
		}

		CU::Vector2i middleOfWindow = windowSize / 2 + windowOffset;
		CU::Vector2i newWindowsMousePos(myDInputWrapper->GetMousePositionX(), myDInputWrapper->GetMousePositionY());

		CU::Vector2f windowSizeF(windowSize);

		CU::Vector2f mouseDelta(newWindowsMousePos - middleOfWindow);
		CU::Vector2f mouseDeltaNormalized(mouseDelta / windowSizeF);

		CU::Vector2f mousePosition(myLastMousePosition + mouseDeltaNormalized);
		mousePosition.Clamp(CU::Vector2f::Zero, CU::Vector2f::One);

		myLastMousePosition = mousePosition;

		if (mouseDeltaNormalized != CU::Vector2f::Zero)
		{
			CU::SInputMessage mouseMoved;
			mouseMoved.myType = CU::eInputType::eMouseMoved;
			mouseMoved.myMouseDelta = mouseDeltaNormalized;

			for (CU::CInputMessenger* messenger : myMessengers)
			{
				if (messenger->RecieveInput(mouseMoved) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}

		for (int i = 0; i < static_cast<int>(CU::eMouseButtons::LENGTH); ++i)
		{
			CU::eMouseButtons mouseButton = static_cast<CU::eMouseButtons>(i);
			if (myDInputWrapper->IsMouseButtonPressed(mouseButton) == true)
			{
				CU::SInputMessage mousePressed;
				mousePressed.myType = CU::eInputType::eMousePressed;
				mousePressed.myMousePosition = myLastMousePosition;
				mousePressed.myMouseButton = mouseButton;

				for (CU::CInputMessenger* messenger : myMessengers)
				{
					if (messenger->RecieveInput(mousePressed) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}

			}
			if (myDInputWrapper->IsMouseButtonReleased(static_cast<CU::eMouseButtons>(i)) == true)
			{
				CU::SInputMessage mouseReleased;
				mouseReleased.myType = CU::eInputType::eMouseReleased;
				mouseReleased.myMousePosition = myLastMousePosition;
				mouseReleased.myMouseButton = mouseButton;

				for (CU::CInputMessenger* messenger : myMessengers)
				{
					if (messenger->RecieveInput(mouseReleased) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}
		}

		int mouseWheelPosition = myDInputWrapper->GetMouseWheelPos();
		if (mouseWheelPosition != myLastMouseWheelPosition)
		{
			if (mouseWheelPosition != 0)
			{
				CU::SInputMessage mouseWheelChanged;
				mouseWheelChanged.myMouseWheelDelta.x = myLastMouseWheelPosition - mouseWheelPosition;
				mouseWheelChanged.myMouseWheelDelta.y = myLastMouseWheelPosition - mouseWheelPosition;
				mouseWheelChanged.myType = CU::eInputType::eScrollWheelChanged;

				for (CU::CInputMessenger* messenger : myMessengers)
				{
					if (messenger->RecieveInput(mouseWheelChanged) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}

			myLastMouseWheelPosition = mouseWheelPosition;
		}

		myDInputWrapper->SetMousePosition(middleOfWindow.x, middleOfWindow.y);
	}
}

void CInputManager::UpdateKeyboard()
{
	if (myDInputWrapper->GetKeysPressed(myKeys) == true)
	{
		for (unsigned int i = 0; i < myKeys.Size(); ++i)
		{
			CU::SInputMessage keyPressed;
			keyPressed.myType = CU::eInputType::eKeyboardPressed;
			keyPressed.myKey = myKeys[i];

			for (CU::CInputMessenger* messenger : myMessengers)
			{
				if (messenger->RecieveInput(keyPressed) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}
	}

	if (myDInputWrapper->GetKeysReleased(myKeys) == true)
	{
		for (unsigned int i = 0; i < myKeys.Size(); ++i)
		{
			CU::SInputMessage keyReleased;
			keyReleased.myType = CU::eInputType::eKeyboardReleased;
			keyReleased.myKey = myKeys[i];

			for (CU::CInputMessenger* messenger : myMessengers)
			{
				if (messenger->RecieveInput(keyReleased) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}
	}
}

void CInputManager::UpdateGamePad(const CU::Time& aDeltaTime)
{
	//delta time is needed for measuring how long a controller is supposed to be vibrating
	for (unsigned i = 0; i < myXInputWrapper->GetConnectedJoystickCount(); ++i)
	{
		if (myXInputWrapper->GetKeyEvents(i, myPadInputs) == true)
		{
			for (int j = 0; j < myPadInputs.Size(); j++)
			{
				CU::SInputMessage padInputsPressed;
				padInputsPressed.myType = (myPadInputs[j].isReleased) ? CU:: eInputType::eGamePadButtonReleased : CU::eInputType::eGamePadButtonPressed;
				padInputsPressed.myGamePad = myPadInputs[j].button;
				padInputsPressed.myGamepadIndex = i;

				for (CU::CInputMessenger* messenger : myMessengers)
				{
					if (messenger->RecieveInput(padInputsPressed) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}
		}
		
		bool leftIsDead = myXInputWrapper->LeftStickIsInDeadzone(i);
		if (!leftIsDead)
		{
			CU::SInputMessage padJoyStick;
			padJoyStick.myType = CU::eInputType::eGamePadLeftJoyStickChanged;
			padJoyStick.myJoyStickPosition = myXInputWrapper->GetLeftStickPosition(i);
			padJoyStick.myGamepadIndex = i;

			for (CU::CInputMessenger* messenger : myMessengers)
			{
				if (messenger->RecieveInput(padJoyStick) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}
		else if (!myXInputWrapper->LeftStickWasInDeadzone(i))
		{
			for (CU::CInputMessenger* messenger : myMessengers)
			{
				CU::SInputMessage padJoyStick;
				padJoyStick.myType = CU::eInputType::eGamePadLeftJoyStickChanged;
				padJoyStick.myGamepadIndex = i;
				if (messenger->RecieveInput(padJoyStick) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}


		if (myXInputWrapper->GetLeftTriggerChanged(i) > 0)
		{
			for (CU::CInputMessenger* messenger : myMessengers)
			{
				CU::SInputMessage padJoyStick;
				padJoyStick.myGamepadIndex = i;
				padJoyStick.myType = CU::eInputType::eGamePadLeftTriggerPressed;
				if (messenger->RecieveInput(padJoyStick) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}


			float test = myXInputWrapper->GetRightTriggerChanged(i);
			if (test > 0)
			{
				for (CU::CInputMessenger* messenger : myMessengers)
				{
					CU::SInputMessage padJoyStick;
					padJoyStick.myGamepadIndex = i;
					padJoyStick.myType = CU::eInputType::eGamePadRightTriggerPressed;
					if (messenger->RecieveInput(padJoyStick) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}

			if (myXInputWrapper->GetLeftTriggerReleased(i) == true)
			{
				for (CU::CInputMessenger* messenger : myMessengers)
				{
					CU::SInputMessage padJoyStick;
					padJoyStick.myGamepadIndex = i;
					padJoyStick.myType = CU::eInputType::eGamePadLeftTriggerReleased;
					if (messenger->RecieveInput(padJoyStick) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}
		
			if (myXInputWrapper->GetRightTriggerReleased(i) == true)
			{
				for (CU::CInputMessenger* messenger : myMessengers)
				{
					CU::SInputMessage padJoyStick;
					padJoyStick.myGamepadIndex = i;
					padJoyStick.myType = CU::eInputType::eGamePadRightTriggerReleased;
					if (messenger->RecieveInput(padJoyStick) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}
			SControllerVibrationState* state = &myControllerVibrationStates[i];
			if (state->myShouldRumbleForever == true)
			{
				state->myTimeIHaveRumbled += aDeltaTime.GetSeconds();
				if (state->myTimeIHaveRumbled >= 2.5f)
				{
					state->myTimeIHaveRumbled -= 2.5f;
					myXInputWrapper->SetLeftVibration(state->myController, state->myLeftIntensity);
					myXInputWrapper->SetRightVibration(state->myController, state->myRightIntensity);
				}
				continue;
			}
			if (state->myTimeToRumble > 0)
			{
				state->myTimeToRumble -= aDeltaTime.GetSeconds();
				if (state->myTimeToRumble <= 0)
				{
					myXInputWrapper->StopVibration(state->myController);
					continue;
				}
				state->myTimeIHaveRumbled += aDeltaTime.GetSeconds();
				if (state->myTimeIHaveRumbled >= 3.0f)
				{
					state->myTimeIHaveRumbled -= 3.0f;
					myXInputWrapper->SetLeftVibration(state->myController, state->myLeftIntensity);
					myXInputWrapper->SetRightVibration(state->myController, state->myRightIntensity);
				}
			}
	}
}
