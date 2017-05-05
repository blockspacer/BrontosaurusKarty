#pragma once

namespace CU
{
	enum class eMouseButtons;
	enum class eKeys;
	enum class GAMEPAD;

	enum class eInputType
	{
		eMouseMoved,
		eMousePressed,
		eMouseReleased,
		eScrollWheelChanged,
		
		eKeyboardPressed,
		eKeyboardReleased,

		eGamePadButtonPressed,
		eGamePadButtonReleased,
		eGamePadLeftJoyStickPressed,
		eGamePadLeftJoyStickReleased,
		eGamePadLeftJoyStickChanged,
		eGamePadRightJoyStickPressed,
		eGamePadRightJoyStickReleased,
		eGamePadRightJoyStickChanged,
		eGamePadLeftTriggerChanged,
		eGamePadLeftTriggerReleased,
		eGamePadLeftTriggerPressed,
		eGamePadRightTriggerChanged,
		eGamePadRightTriggerReleased,
		eGamePadRightTriggerPressed,
	};

	struct SInputMessage
	{
		SInputMessage() : myType(eInputType::eMouseMoved), myMousePosition(0.f, 0.f) {}
		union
		{
			Vector2f myMousePosition;
			Vector2f myMouseDelta;
			Vector2f myMouseWheelDelta;
			Vector2f myJoyStickPosition;
		};

		union
		{
			eMouseButtons myMouseButton;
			eKeys myKey;
			GAMEPAD myGamePad;
		};

		eInputType myType;
	};
}

static_assert(sizeof(CU::SInputMessage) <= sizeof(CU::Vector2f) * 2, "Allignement is fucked, enums different sizes?");
