#pragma once
#include "Component.h"

class CHazardComponent : public CComponent
{
public:
	CHazardComponent();
	~CHazardComponent();

	void SetToPermanent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	bool myIsActive;
	bool myIsPermanent;
};

