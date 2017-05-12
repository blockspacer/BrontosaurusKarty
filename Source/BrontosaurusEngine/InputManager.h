#pragma once

#include "../PostMaster/Subscriber.h"
#include "../ThreadedPostmaster/Subscriber.h"

namespace CU
{
	class InputWrapper;
	class XInputWrapper;

	class CInputMessenger;

	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;

	enum class eKeys;
	enum class GAMEPAD : short;

	struct KeyEvent;
}

struct SControllerVibrationState
{
	float myTimeToRumble;
	float myTimeIHaveRumbled;
	unsigned int myController;
	unsigned short myLeftIntensity;
	unsigned short myRightIntensity;
	bool myShouldRumbleForever;
};

class CInputManager : public Postmaster::ISubscriber
{
public:
	CInputManager();
	~CInputManager();

	void Update(const CU::Time& aDeltaTime);
	void SetMousePosition(const CU::Vector2f& aMousePosition);
	void LockUnlockMouse(const bool aHasFocus);
	void Listen(CU::CInputMessenger& aMessenger, const int aPriority);
	void Neglect(CU::CInputMessenger& aMessenger);

	int AddXboxController();

	eMessageReturn DoEvent(const FocusChange& aDroppedFile) override;
	eMessageReturn DoEvent(const SetVibrationOnController& aSetVibration) override;
	eMessageReturn DoEvent(const StopVibrationOnController& aStopVibrationmessage) override;

	static CInputManager* GetInstance();

private:
	void UpdateMouse();
	void UpdateKeyboard();
	void UpdateGamePad(const CU::Time& aDeltaTime);


	CU::GrowingArray<CU::eKeys> myKeys;
	CU::GrowingArray<CU::KeyEvent> myPadInputs;
	CU::GrowingArray<CU::CInputMessenger*> myMessengers;

	CU::Vector2f myLastMousePosition;
	CU::InputWrapper* myDInputWrapper;
	CU::XInputWrapper* myXInputWrapper;

	CU::GrowingArray<SControllerVibrationState> myControllerVibrationStates;

	int myLastMouseWheelPosition;

	bool myHasFocus;

	static CInputManager* ourInstance;
};
