#include "stdafx.h"
#include "VertexStreamComponent.h"

Component::CVertexStreamComponent::CVertexStreamComponent(const std::string& aPath) : myPath(aPath)
{
	myType = eComponentType::eVertexStream;
}

Component::CVertexStreamComponent::~CVertexStreamComponent()
{
}

void Component::CVertexStreamComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CComponent::Receive(aMessageType, aMessageData);
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if(aMessageData.myComponentTypeAdded == eComponentType::eModel)
		{
			SComponentMessageData messageData;
			messageData.myComponentTypeAdded = eComponentType::eVertexStream;
			GetParent()->NotifyComponents(eComponentMessageType::eAddComponent, messageData);
		}
		break;
	}
}

const std::string& Component::CVertexStreamComponent::GetMapPath() const
{
	return myPath;
}
