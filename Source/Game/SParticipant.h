#pragma once
struct SParticipant
{
	enum class eInputDevice
	{
		//den som ändrar här dör. Best regards Johan
		eController1 = 0,
		eController2,
		eController3,
		eController4,
		eKeyboard,
		eNone
	};
	enum class eCharacter
	{
		eVanBrat,
		eVanBrat2,
		eGrandMa,
		eGrandMa2,
	};
	eInputDevice myInputDevice;
	eCharacter mySelectedCharacter;
};