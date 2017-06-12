#pragma once
#include "Component.h"

class CScene;
typedef unsigned int InstanceID;

class CDecalComponent :	public CComponent
{
public:
	CDecalComponent(CScene& aScene);
	~CDecalComponent();

	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CScene& myScene;
	InstanceID myID;
};
