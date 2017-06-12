#pragma once
#include "EMessageReturn.h"
enum class eMessageType;

namespace Postmaster
{
	class ISubscriber;
	namespace Message
	{
		class IMessage
		{
		public:
			explicit IMessage(eMessageType aType);
			virtual ~IMessage();

			virtual IMessage* Copy() = 0;

			virtual eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const = 0;

			eMessageType GetType() const;

			void SetCallback(const std::function<void(void)>& aCallback);
			void DoCallback() const;
		private:
			eMessageType myType;
			std::function<void()> myCallback;
		};
	}
}

//This initialization is probably the best thing ever!
inline Postmaster::Message::IMessage::IMessage(eMessageType aType) : myType(aType), myCallback([](){})
{
}

inline Postmaster::Message::IMessage::~IMessage()
{
}

inline eMessageType Postmaster::Message::IMessage::GetType() const
{
	return myType;
}

inline void Postmaster::Message::IMessage::SetCallback(const std::function<void()>& aCallback)
{
	myCallback = aCallback;
}

inline void Postmaster::Message::IMessage::DoCallback() const
{
	myCallback();
}
