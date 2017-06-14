#pragma once
struct SParticipant
{
	bool operator== (const SParticipant& left)
	{
		if (myInputDevice != left.myInputDevice)
		{
			return false;
		}
		if (myIsReady != left.myIsReady)
		{
			return false;
		}
		if (mySelectedCharacter != left.mySelectedCharacter)
		{
			return false;
		}
		return true;
	}
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
		eCat,
		eVanBrat,
		eGrandMa,
		eCat2,
		eVanBrat2,
		eGrandMa2,
		eLength,
	};
	eInputDevice myInputDevice;
	eCharacter mySelectedCharacter;
	bool myIsReady;
};