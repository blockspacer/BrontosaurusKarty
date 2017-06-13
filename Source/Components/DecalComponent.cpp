#include "stdafx.h"
#include "DecalComponent.h"

#include "..\BrontosaurusEngine\DecalInstance.h"
#include "..\BrontosaurusEngine\Scene.h"


CDecalComponent::CDecalComponent(CScene& aScene)
	: myScene(aScene)
{
	myID = aScene.AddDecal();
}

CDecalComponent::~CDecalComponent()
{
}

void CDecalComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CDecalInstance* decal = myScene.GetDecal(myID);
	if (!decal)
	{
		return;
	}

	switch (aMessageType)
	{
	case eComponentMessageType::eObjectDone:
		//decal->SetSize(1.f, 10.f, 10.f);
	case eComponentMessageType::eMoving:
		decal->GetTransformation() = GetParent()->GetToWorldTransform();
		break;
	}
}

void CDecalComponent::SetDecalIndex(const unsigned int decalIndex)
{
	CDecalInstance* decal = myScene.GetDecal(myID);
	if (!decal)
	{
		return;
	}
	decal->SetDecalIndex(decalIndex);
}
