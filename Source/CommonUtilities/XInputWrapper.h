#pragma once

struct _XINPUT_STATE;
typedef _XINPUT_STATE XINPUT_STATE;

namespace CU
{
	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;

	enum class GAMEPAD
	{
		DPAD_UP = 0x0001,
		DPAD_DOWN = 0x0002,
		DPAD_LEFT = 0x0004,
		DPAD_RIGHT = 0x0008,
		START = 0x0010,
		BACK = 0x0020,
		LEFT_THUMB = 0x0040,
		RIGHT_THUMB = 0x0080,
		LEFT_SHOULDER = 0x0100,
		RIGHT_SHOULDER = 0x0200,
		A = 0x1000,
		B = 0x2000,
		X = 0x4000,
		Y = 0x8000
	};

	struct TriggerState
	{
		float LeftLastFrame = 0;
		float RightLastFrame = 0;
		bool LeftReleased = false;
		bool RightReleased = false;
	};

	struct KeyEvent
	{
		GAMEPAD button;
		bool isReleased;
	};

	class XInputWrapper
	{
	public:
		struct KeyStroke
		{
			enum class eKeyState : unsigned short
			{
				ePressed,
				eReleased,
				eRepeat

			} myKeyState;

			unsigned short myKeyCode;
		};

		XInputWrapper();
		~XInputWrapper();

		void Init(const unsigned int aJoystickCount);

		void UpdateStates();

		//bool GetKeyStroke(const unsigned int aJoystickIndex, KeyStroke& aKeyStrokeOutput);
		//bool GetKeyPressed(const unsigned int aJoystickIndex, const unsigned short aButton);
		//bool GetKeyReleased(const unsigned int aJoystickIndex, const unsigned short aButton);

		bool GetKeyEvents(const unsigned int aJoystickIndex, CU::GrowingArray<KeyEvent>& aKeys);

		bool IsConnected(const unsigned int aJoystickIndex, unsigned int* aError = nullptr);

		CU::Vector2f GetRightStickPosition(const unsigned int aJoystickIndex);
		CU::Vector2f GetLeftStickPosition(const unsigned int aJoystickIndex);
		bool LeftStickIsInDeadzone(const unsigned int aJoystickIndex);
		bool RightStickIsInDeadzone(const unsigned int aJoystickIndex);
		bool LeftStickWasInDeadzone(const unsigned int aJoystickIndex);
		bool RightStickWasInDeadzone(const unsigned int aJoystickIndex);

		float GetLeftTriggerDown(const unsigned int aJoystickIndex);
		float GetLeftTriggerChanged(const unsigned int aJoystickIndex);
		bool GetLeftTriggerReleased(const unsigned int aJoystickIndex);

		float GetRightTringgerDown(const unsigned int aJoystickIndex);
		float GetRightTriggerChanged(const unsigned int aJoystickIndex);
		bool GetRightTriggerReleased(const unsigned int aJoystickIndex);

		void SetLeftVibration(const unsigned int aJoystickIndex, const unsigned short aAmount);
		void SetRightVibration(const unsigned int aJoystickIndex, const unsigned short aAmount);
		void SetFullControllerVibration(const unsigned int aJoystickIndex, const unsigned short aAmount);
		void StopVibration(const unsigned int aJoystickIndex);

		unsigned int GetConnectedJoystickCount() const;

	private:
		bool UpdateState(const unsigned int aJoystickIndex);
		
		GrowingArray<XINPUT_STATE> myJoysticks;
		GrowingArray<unsigned short> myPreviousButtonState;
		GrowingArray<struct SJoystickDead> myPreviousJoystickStates;
		GrowingArray<TriggerState> myPreviousTriggerStates;

	};
}
