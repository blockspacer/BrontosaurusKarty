#pragma once
#include "Component.h"

class CGoalComponent : public CComponent
{
public:
	CGoalComponent();
	~CGoalComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
};

