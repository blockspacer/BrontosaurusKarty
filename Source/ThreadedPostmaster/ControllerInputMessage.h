#pragma once
#include "Message.h"

namespace Postmaster
{
	namespace Message
	{
		enum class EventType
		{
			MovedJoystick,
			ButtonChanged,
			None
		};

		enum class ButtonIndex
		{
			RightStick,
			LeftStick,
			DPad,
			X, Y, B, A,
			Start,
			Back,
			LTrigger,
			LShoulder,
			RTrigger,
			RShoulder,
			None
		};
		union DataUnion
		{
			bool boolValue;


			float floatValue;
			int intValue;

		};
		
		struct InputEventData
		{

			EventType eventType;
			ButtonIndex buttonIndex;
			DataUnion data;
		};
		class CControllerInputMessage : public IMessage
		{
		public:
			CControllerInputMessage(short aControllerIndex, const InputEventData& aEventData);
			~CControllerInputMessage();
			IMessage* Copy() override;
			eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
			const InputEventData& GetData() const;
		protected:
			short myControllerIndex;
			InputEventData myEventData;
		};
	}
}

