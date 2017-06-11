#include "stdafx.h"
#include "GamepadButtons.h"

const CU::StaticArray<const char*, 14> CU::GamePadButtonNames = { 
	"DPAD_UP",
	"DPAD_DOWN", 
	"DPAD_LEFT", 
	"DPAD_RIGHT", 
	"START", 
	"BACK", 
	"LEFT_THUMB", 
	"RIGHT_THUMB", 
	"LEFT_SHOULDER", 
	"RIGHT_SHOULDER", 
	"A", 
	"B", 
	"X", 
	"Y" 
};

const CU::StaticArray<CU::GAMEPAD, 14> CU::GamePadButtons = {
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