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
#define THROW_AWAY_IF_LOW(LOW_VALUE, REAL_VALUE) ((std::abs(REAL_VALUE) - LOW_VALUE <= 0) ? REAL_VALUE : 0)

	const unsigned int NullJoystick = static_cast<unsigned int>(-1);

	struct SJoystickDead
	{
		bool left = true;
		bool right = true;
	};

	XInputWrapper::XInputWrapper()
		: myJoysticks(4)
		, myPreviousButtonState(4)
		, myPreviousJoystickStates(4)
		, myPreviousTriggerStates(4)
	{
	}

	XInputWrapper::~XInputWrapper()
	{
	}

	void XInputWrapper::Init(const unsigned int aJoystickCount)
	{
		//myJoysticks.Resize(aJoystickCount);
		//myPreviousButtonState.Resize(aJoystickCount);
		//myPreviousJoystickStates.Resize(aJoystickCount);
		//myPreviousTriggerStates.Resize(aJoystickCount);
	}

	void XInputWrapper::UpdateStates()
	{
		for (unsigned int i = 0; i < myJoysticks.Size(); ++i)
		{
			if (myJoysticks[i].dwPacketNumber != NullJoystick)
			{
				if (UpdateState(i) == false)
				{
					myJoysticks[i].dwPacketNumber = NullJoystick;
				}
			}
		}
	}

	bool XInputWrapper::GetKeyEvents(const unsigned int aJoystickIndex, CU::GrowingArray<KeyEvent>& aKeys)
	{
		aKeys.RemoveAll();

		for (unsigned short i = 0; i < 14; ++i)
		{
			unsigned short downNow = (1 << i) & myJoysticks[aJoystickIndex].Gamepad.wButtons;
			unsigned short downBefore = (1 << i) & myPreviousButtonState[aJoystickIndex];
			if (downNow && !downBefore)
			{
				aKeys.Add({ static_cast<GAMEPAD>(downNow), false });
			}
			else if (!downNow && downBefore)
			{
				aKeys.Add({ static_cast<GAMEPAD>(downBefore), true });
			}
		}

		return aKeys.Empty() == false;
	}

	bool XInputWrapper::IsConnected(const unsigned int aJoystickIndex, unsigned int* aError)
	{
		XINPUT_STATE joystickOnStack;
		XINPUT_STATE* joystick = &joystickOnStack;
		unsigned int joystickIndex = myJoysticks.Size();
		if (aJoystickIndex < joystickIndex)
		{
			joystick = &myJoysticks[aJoystickIndex];
			joystickIndex = aJoystickIndex;
		}
		ZeroMemory(joystick, sizeof(XINPUT_STATE));

		DWORD result = XInputGetState(joystickIndex, joystick);

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

	int XInputWrapper::AddController()
	{
		unsigned int index = myJoysticks.Size();
		if (IsConnected(index))
		{
			myJoysticks.Add();
			myPreviousButtonState.Add();
			myPreviousJoystickStates.Add();
			myPreviousTriggerStates.Add();
			return static_cast<int>(index);
		}

		return -1;
	}

	CU::Vector2f XInputWrapper::GetRightStickPosition(const unsigned int aJoystickIndex)
	{
		CU::Vector2f position(myJoysticks[aJoystickIndex].Gamepad.sThumbRX, myJoysticks[aJoystickIndex].Gamepad.sThumbRY);

		if (position.Length2() < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			position = CU::Vector2f::Zero;
		}

		return position / SHRT_MAX;
	}

	CU::Vector2f XInputWrapper::GetLeftStickPosition(const unsigned int aJoystickIndex)
	{
		CU::Vector2f position(myJoysticks[aJoystickIndex].Gamepad.sThumbLX, myJoysticks[aJoystickIndex].Gamepad.sThumbLY);

		if (position.Length2() < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			position = CU::Vector2f::Zero;
		}

		return position / SHRT_MAX;
	}

	bool XInputWrapper::LeftStickIsInDeadzone(const unsigned int aJoystickIndex)
	{
		short x = myJoysticks[aJoystickIndex].Gamepad.sThumbLX;
		short y = myJoysticks[aJoystickIndex].Gamepad.sThumbLY;

		bool isDead = std::abs(x) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && std::abs(y) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		//return THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, x) == 0 && THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, y) == 0;
		return isDead;
	}

	bool XInputWrapper::RightStickIsInDeadzone(const unsigned int aJoystickIndex)
	{
		short x = myJoysticks[aJoystickIndex].Gamepad.sThumbRX;
		short y = myJoysticks[aJoystickIndex].Gamepad.sThumbRY;

		bool isDead = std::abs(x) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && std::abs(y) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		//return THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, x) == 0 && THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, y) == 0;
		return isDead;
	}

	bool XInputWrapper::LeftStickWasInDeadzone(const unsigned int aJoystickIndex)
	{
		return myPreviousJoystickStates[aJoystickIndex].left;
	}

	bool XInputWrapper::RightStickWasInDeadzone(const unsigned int aJoystickIndex)
	{
		return myPreviousJoystickStates[aJoystickIndex].right;
	}

	float XInputWrapper::GetLeftTriggerDown(const unsigned int aJoystickIndex)
	{
		unsigned char trigger = myJoysticks[aJoystickIndex].Gamepad.bLeftTrigger;

		return static_cast<float>(THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_TRIGGER_THRESHOLD, trigger)) / static_cast<float>(UCHAR_MAX);
	}

	float XInputWrapper::GetRightTringgerDown(const unsigned int aJoystickIndex)
	{
		unsigned char trigger = myJoysticks[aJoystickIndex].Gamepad.bRightTrigger;

		float result = static_cast<float>(THROW_AWAY_IF_LOW(XINPUT_GAMEPAD_TRIGGER_THRESHOLD, trigger)) / static_cast<float>(UCHAR_MAX);

		return result;
	}

	float XInputWrapper::GetLeftTriggerChanged(const unsigned int aJoystickIndex)
	{

		unsigned char trigger = myJoysticks[aJoystickIndex].Gamepad.bLeftTrigger;

		float result = static_cast<float>(THROW_AWAY_IF_LOW(255, trigger)) / static_cast<float>(UCHAR_MAX);

		if (myPreviousTriggerStates[aJoystickIndex].LeftLastFrame > 0 && result == 0)
		{
			myPreviousTriggerStates[aJoystickIndex].LeftReleased = true;
		}
		else
		{
			myPreviousTriggerStates[aJoystickIndex].LeftReleased = false;
		}
		myPreviousTriggerStates[aJoystickIndex].LeftLastFrame = result;

		return result;
	}

	bool XInputWrapper::GetLeftTriggerReleased(const unsigned int aJoystickIndex)
	{
		return myPreviousTriggerStates[aJoystickIndex].LeftReleased;
	}

	float XInputWrapper::GetRightTriggerChanged(const unsigned int aJoystickIndex)
	{
		unsigned char trigger = myJoysticks[aJoystickIndex].Gamepad.bRightTrigger;

		float result = static_cast<float>(THROW_AWAY_IF_LOW(255, trigger)) / static_cast<float>(UCHAR_MAX);

		if (myPreviousTriggerStates[aJoystickIndex].RightLastFrame > 0 && result == 0)
		{
			myPreviousTriggerStates[aJoystickIndex].RightReleased = true;
		}
		else
		{
			myPreviousTriggerStates[aJoystickIndex].RightReleased = false;
		}
		myPreviousTriggerStates[aJoystickIndex].RightLastFrame = result;

		return result;
	}

	bool XInputWrapper::GetRightTriggerReleased(const unsigned int aJoystickIndex)
	{
		return myPreviousTriggerStates[aJoystickIndex].RightReleased;
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
		unsigned int size = myJoysticks.Size();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (myJoysticks[i].dwPacketNumber == NullJoystick)
			{
				size--;
			}
		}
		if (size == 0)
		{
			const_cast<XInputWrapper*>(this)->myJoysticks.RemoveAll();
		}

		return size;
	}

	bool XInputWrapper::UpdateState(const unsigned int aJoystickIndex)
	{
		myPreviousButtonState[aJoystickIndex] = myJoysticks[aJoystickIndex].Gamepad.wButtons;
		myPreviousJoystickStates[aJoystickIndex].left = LeftStickIsInDeadzone(aJoystickIndex);
		myPreviousJoystickStates[aJoystickIndex].right = RightStickIsInDeadzone(aJoystickIndex);

		ZeroMemory(&myJoysticks[aJoystickIndex], sizeof(XINPUT_STATE));

		unsigned int result = XInputGetState(aJoystickIndex, &myJoysticks[aJoystickIndex]);
		if (result == ERROR_SUCCESS)
		{
			return true;
		}
		else if (result == ERROR_DEVICE_NOT_CONNECTED)
		{
		}

		return false;
	}
}
