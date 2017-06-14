#include "stdafx.h"
#include "CreditsState.h"

#include "../CommonUtilities/InputMessage.h"
#include "../CommonUtilities/EInputReturn.h"

#include "../BrontosaurusEngine/SpriteInstance.h"

CCreditsState::CCreditsState(StateStack& aStateStack)
	: State(aStateStack, eInputMessengerType::eCredits, 1)
{
}

CCreditsState::~CCreditsState()
{
}

void CCreditsState::Init()
{
	myCreditsSprite = std::make_unique<CSpriteInstance>("Sprites/Credits/credits.dds");
	myCreditsSprite->SetPivot({ 0.5f, 0.5f });
	myCreditsSprite->SetPosition({ 0.5f, 0.5f });
}

eStateStatus CCreditsState::Update(const CU::Time& /*aDeltaTime*/)
{
	return myStatus;
}

void CCreditsState::Render()
{
	myCreditsSprite->Render();
}

void CCreditsState::OnEnter(const bool /*aLetThroughRender*/)
{
}

void CCreditsState::OnExit(const bool /*aLetThroughRender*/)
{
}

CU::eInputReturn CCreditsState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eMousePressed ||
		aInputMessage.myType == CU::eInputType::eGamePadButtonPressed ||
		aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		SetStateStatus(eStateStatus::ePop);
	}

	return CU::eInputReturn::eKeepSecret;
}
