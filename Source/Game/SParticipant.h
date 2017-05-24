#pragma once
struct SParticipant
{
	enum class eInputDevice
	{
		//den som ändrar här dör. Best regards Johan
		eController1 = 0,
		eController2 = 1,
		eController3 = 2,
		eController4 = 3,
		eKeyboard,
		eNone,
	};
	eInputDevice myInputDevice;
};