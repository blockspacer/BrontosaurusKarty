#include "stdafx.h"
#include "XInputWrapper.h"

#include "../CommonUtilities/vector2.h"

#include <Windows.h>
#include <Xinput.h>

#pragma comment (lib, "XInput.lib")

const CU::GAMEPAD GamePadButtons[] = {
	CU::GAMEPAD::DPAD_UP,
	CU::GAMEPAD::DPAD_DOWN,
	CU::GAMEPAD::DPAD_LEFT,
	CU::GAMEPAD::DPAD_RIGHT,
	CU::GAMEPAD::START,
	CU::GAMEPAD::BACK,
	CU::GAMEPAD::LEFT_THUMB,
	CU::GAMEPAD::RIGHT_THUMB,
	CU::GAMEPAD::LEFT_SHOULDER,
	CU::GAMEPAD::RIGHT_SHOULDER,
	CU::GAMEPAD::A ,
	CU::GAMEPAD::B,
	CU::GAMEPAD::X,
	CU::GAMEPAD::Y
};

namespace CU
{
	typedef GrowingArray<XINPUT_STATE*>::size_type size_g;

#define THROW_AWAY_IF_LOW(LOW_VALUE, REAL_VALUE) ((std::abs(REAL_VALUE) - LOW_VALUE <= 0) ? REAL_VALUE : 0)

	XInputWrapper::KeyStroke::eKeyState FlagsToKeyState(const WORD aFlag)
	{
		DL_PRINT("%d", aFlag);
		if (aFlag & XINPUT_KEYSTROKE_REPEAT)
		{
			return XInputWrapper::KeyStroke::eKeyState::eRepeat;
		}
		if (aFlag & XINPUT_KEYSTROKE_KEYUP)
		{
			return XInputWrapper::KeyStroke::eKeyState::eReleased;
		}
		if (aFlag & XINPUT_KEYSTROKE_KEYDOWN)
		{
			return XInputWrapper::KeyStroke::eKeyState::ePressed;
		}
	}

	XInputWrapper::XInputWrapper()
		: myJoysticks(2)
		, myDisconnectedJoysticks(2)
	{
	}

	XInputWrapper::~XInputWrapper()
	{
		myJoysticks.DeleteAll();

		if (myDisconnectedJoysticks.Size() > 0)
		{
			//post problem message?
			myDisconnectedJoysticks.DeleteAll();
		}
	}

	void XInputWrapper::Init(const unsigned int aJoystickCount)
	{
		for (size_g i = 0; i < aJoystickCount; ++i)
		{
			myJoysticks.Add(new XINPUT_STATE());
		}
	}

	void XInputWrapper::UpdateStates()
	{
		for (size_g i = 0; i < myJoysticks.Size(); ++i)
		{
			if (UpdateState(i) == false)
			{
				myDisconnectedJoysticks.Add(myJoysticks[i]);
			}
		}

		MoveDisconnectedJoysticks();
	}

	bool XInputWrapper::GetKeyStroke(const unsigned int aJoystickIndex, KeyStroke& aKeyStrokeOutput)
	{
		XINPUT_KEYSTROKE keyStroke = {};

		unsigned int result = XInputGetKeystroke(aJoystickIndex, 0, &keyStroke);
		if (result == ERROR_SUCCESS)
		{
			aKeyStrokeOutput.myKeyState = FlagsToKeyState(keyStroke.Flags);
			aKeyStrokeOutput.myKeyCode = myJoysticks[aJoystickIndex]->Gamepad.wButtons;//keyStroke.VirtualKey;
			return true;
		}
		else if (result == ERROR_DEVICE_NOT_CONNECTED)
		{
			myDisconnectedJoysticks.Add(myJoysticks[aJoystickIndex]);
		}

		return false;
	}

	bool XInputWrapper::GetKeyPressed(const unsigned int aJoystickIndex, const unsigned short aButton)
	{
		KeyStroke keyStroke = {};
		if (GetKeyStroke(aJoystickIndex, keyStroke) == true)
		{
			DL_PRINT("%i", keyStroke.myKeyCode);
			if (keyStroke.myKeyCode == aButton && keyStroke.myKeyState == KeyStroke::eKeyState::ePressed)
			{
				return true;
			}

			for (int i = 0; i < 14; ++i)
			{
				if (keyStroke.myKeyCode & static_cast<unsigned short>(GamePadButtons[i]) && keyStroke.myKeyState == KeyStroke::eKeyState::ePressed)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool XInputWrapper::GetKeyReleased(const unsigned int aJoystickIndex, const unsigned short aButton)
	{
		KeyStroke keyStroke = {};
		if (GetKeyStroke(aJoystickIndex, keyStroke) == true)
		{
			if (keyStroke.myKeyCode & aButton && keyStroke.myKeyState == KeyStroke::eKeyState::eReleased)
			{
				return true;
			}
		}

		return false;
	}

	bool XInputWrapper::GetKeyEvents(const unsigned int aJoystickIndex, CU::GrowingArray<KeyEvent>& aKeys)
	{
		aKeys.RemoveAll();

		KeyStroke keyStroke = {};
		if (GetKeyStroke(aJoystickIndex, keyStroke) == true)
		{
			for (int i = 0; i < 14; ++i)
			{
				CU::GAMEPAD button = GamePadButtons[i];

				if (keyStroke.myKeyCode & static_cast<unsigned short>(button))
				{
					if (keyStroke.myKeyState == KeyStroke::eKeyState::ePressed)
					{
						aKeys.Add({ button , false });
					}
					else if (keyStroke.myKeyState == KeyStroke::eKeyState::eReleased)
					{
						aKeys.Add({ button, true });
					}
				}
			}
		}
		//for (unsigned int i = 0; i < 14; ++i)
		//{
		//	CU::GAMEPAD button = GamePadButtons[i];
		//	if (GetKeyPressed(aJoystickIndex, static_cast<unsigned short>(button)))
		//	{
		//		aKeys.Add(button);
		//	}
		//}

		return aKeys.Empty() == false;
	}

	//bool XInputWrapper::GetKeysReleased(const unsigned int aJoystickIndex, CU::GrowingArray<KeyEvent>& aKeys)
	//{
	//	aKeys.RemoveAll();

	//	KeyStroke keyStroke = {};
	//	if (GetKeyStroke(aJoystickIndex, keyStroke) == true)
	//	{
	//		for (int i = 0; i < 14; ++i)
	//		{
	//			CU::GAMEPAD button = GamePadButtons[i];

	//			if (keyStroke.myKeyCode & static_cast<unsigned short>(button) && keyStroke.myKeyState == KeyStroke::eKeyState::eReleased)
	//			{
	//				aKeys.Add(button);
	//			}
	//		}
	//	}

	//	return aKeys.Empty() == false;
	//}

	bool XInputWrapper::IsConnected(const unsigned int aJoystickIndex, unsigned int* aError)
	{
		ZeroMemory(&myJoysticks[aJoystickIndex], sizeof(XINPUT_STATE));

		DWORD result = XInputGetState(aJoystickIndex, myJoysticks[aJoystickIndex]);

		if (aError != nullptr)
		{
			*aError = result;
		}

		if (result == ERROR_SUCCESS)
		{
			return true;
		}

		return false;
	}

	CU::Vector2f XInputWrapper::GetRightStickPosition(const unsigned int aJoystickIndex)
	{
		CU::Vector2f position(myJoysticks[aJoystickIndex]->Gamepad.sThumbRX, myJoysticks[aJoystickIndex]->Gamepad.sThumbRY);

		if (position.Length2() < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			position = CU::Vector2f::Zero;
		}

		return position / SHRT_MAX;
	}

	CU::Vector2f XInputWrapper::GetLeftStickPosition(const unsigned int aJoystickIndex)
	{
		CU::Vector2f position(myJoysticks[aJoystickIndex]->Gamepad.sThumbLX, myJoysticks[aJoystickIndex]->Gamepad.sThumbLY);

		if (position.Length2() < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			position = CU::Vector2f::Zero;
		}

		return position / SHRT_MAX;
	}

	bool XInputWrapper::LeftStickIsInDeadzone(const unsigned int aJoystickIndex)
	{
		short x = myJoysticks[aJoystickIndex]->Gamepad.sThumbLX;
		short y = myJoysticks[aJoystickIndex]->Gamepad.sThumbLY;

		return THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, x) == 0 && THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, y) == 0;
	}

	bool XInputWrapper::RightStickIsInDeadzone(const unsigned int aJoystickIndex)
	{
		short x = myJoysticks[aJoystickIndex]->Gamepad.sThumbRX;
		short y = myJoysticks[aJoystickIndex]->Gamepad.sThumbRY;

		return THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, x) == 0 && THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, y) == 0;
	}

	float XInputWrapper::GetLeftTriggerDown(const unsigned int aJoystickIndex)
	{
		unsigned char trigger = myJoysticks[aJoystickIndex]->Gamepad.bLeftTrigger;

		return static_cast<float>(THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_TRIGGER_THRESHOLD, trigger)) / static_cast<float>(UCHAR_MAX);
	}

	float XInputWrapper::GetRightTringgerDown(const unsigned int aJoystickIndex)
	{
		unsigned char trigger = myJoysticks[aJoystickIndex]->Gamepad.bRightTrigger;

		return static_cast<float>(THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_TRIGGER_THRESHOLD, trigger)) / static_cast<float>(UCHAR_MAX);
	}

	void XInputWrapper::SetLeftVibration(const unsigned int aJoystickIndex, const unsigned short aAmount)
	{
		XINPUT_VIBRATION vibration = {};

		vibration.wLeftMotorSpeed = aAmount;
		//vibration.wRightMotorSpeed = 0.f;

		XInputSetState(aJoystickIndex, &vibration);
	}

	void XInputWrapper::SetRightVibration(const unsigned int aJoystickIndex, const unsigned short aAmount)
	{
		XINPUT_VIBRATION vibration = {};

		//vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = aAmount;

		XInputSetState(aJoystickIndex, &vibration);
	}

	void XInputWrapper::SetFullControllerVibration(const unsigned int aJoystickIndex, const unsigned short aAmount)
	{
		XINPUT_VIBRATION vibration = {};

		vibration.wLeftMotorSpeed = aAmount;
		vibration.wRightMotorSpeed = aAmount;

		XInputSetState(aJoystickIndex, &vibration);
	}

	void XInputWrapper::StopVibration(const unsigned int aJoystickIndex)
	{
		XINPUT_VIBRATION vibration = {};

		vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = 0;

		XInputSetState(aJoystickIndex, &vibration);
	}

	unsigned int XInputWrapper::GetConnectedJoystickCount() const
	{
		return myJoysticks.Size();
	}

	bool XInputWrapper::UpdateState(const unsigned int aJoystickIndex)
	{
		ZeroMemory(myJoysticks[aJoystickIndex], sizeof(XINPUT_STATE));

		if (XInputGetState(aJoystickIndex, myJoysticks[aJoystickIndex]) == ERROR_SUCCESS)
		{
			return true;
		}

		return false;
	}

	void XInputWrapper::MoveDisconnectedJoysticks()
	{
		for (size_g i = 0; i < myDisconnectedJoysticks.Size(); ++i)
		{
			myJoysticks.Remove(myDisconnectedJoysticks[i]);
		}

		myDisconnectedJoysticks.DeleteAll();
	}
}
