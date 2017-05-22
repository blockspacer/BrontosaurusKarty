#pragma once
#include "Component.h"

class CHazardComponent : public CComponent
{
public:
	CHazardComponent();
	~CHazardComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

};

