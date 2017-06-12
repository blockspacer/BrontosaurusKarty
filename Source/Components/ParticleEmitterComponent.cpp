#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "ParticleEmitterComponentManager.h"
#include "ParticleEmitterManager.h"

#include "../BrontosaurusEngine/ParticleEmitterInstance.h"


CParticleEmitterComponent::CParticleEmitterComponent(int anId)
{
	myInstanceId = anId;
	myType = eComponentType::eParticleEmitter;
	CParticleEmitterManager::GetInstance().Activate(myInstanceId);
}


CParticleEmitterComponent::~CParticleEmitterComponent()
{
	CParticleEmitterManager::GetInstance().Release(myInstanceId);
}

void CParticleEmitterComponent::Update(CU::Time aDeltaTime)
{
	if (myInstanceId == 118)
	{
		int i = 0;
	}
	if (!GetParent())
	{
		return;
	}
	//myParticleInstance->SetPosition(GetParent()->GetToWorldTransform().GetPosition());
	CParticleEmitterManager::GetInstance().SetTransformation(myInstanceId, GetParent()->GetToWorldTransform());
	//myParticleInstance->Update(aDeltaTime);
}
void CParticleEmitterComponent::Activate()
{
	CParticleEmitterManager::GetInstance().Activate(myInstanceId);
}

void CParticleEmitterComponent::Deactivate()
{
	CParticleEmitterManager::GetInstance().Deactivate(myInstanceId);
}


void CParticleEmitterComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eActivateEmitter:
		Activate();
		break;
	case eComponentMessageType::eDeactivateEmitter:
		Deactivate();
		break;
	default: break;
	}
}

void CParticleEmitterComponent::Destroy()
{
	CParticleEmitterComponentManager::GetInstance().Remove(this);
}
