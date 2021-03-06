#include "stdafx.h"
#include "StateStack.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostMaster/MessageType.h"
#include "../ThreadedPostMaster/ConsoleCalledUpon.h"
#include "../ThreadedPostMaster/PushState.h"
#include "../Game/LoadState.h"

StateStack::StateStack()
	: myStates(8)
	, mySwapStateFunction(nullptr)
	, myStateToSwapTo(nullptr)
	, myShouldUpdate(true)
{
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eStateStackMessage);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eConsoleCalledUpon);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eQuitGame);

}

StateStack::~StateStack()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
	Clear();
}

void StateStack::PushState(State *aState)
{
	if(myShouldUpdate == true)
	{
		if (myStates.Size() > 0)
		{
			myStates.GetLast()->OnExit(aState->GetLetThroughRender());
		}

		myStates.Add(aState);
		aState->Init();
		aState->OnEnter(false);
	}
}

State* StateStack::GetCurrentState()
{
	return myStates.GetLast();
}

eMessageReturn StateStack::DoEvent(const ConsoleCalledUpon& aConsoleCalledUpon)
{
	SetShouldUpdate(aConsoleCalledUpon.GetIsConsoleActive() == false);
	return eMessageReturn::eContinue;
}

eMessageReturn StateStack::DoEvent(const PopCurrentState& aPopCurrent)
{
	GetCurrentState()->SetStateStatus(eStateStatus::ePop);
	return eMessageReturn::eStop;
}

eMessageReturn StateStack::DoEvent(const ::PushState& aPushState)
{
	switch (aPushState.GetStateType())
	{
	case PushState::eState::ePlayState:

		PushState(new CLoadState(*this, aPushState.GetLevelIndex()));
		break;
	}

	return eMessageReturn::eStop;
}

eMessageReturn StateStack::DoEvent(const CQuitGame& aQuitGameMessage)
{
	for (State* state : myStates)
	{
		state->SetStateStatus(eStateStatus::ePop);
	}

	return eMessageReturn::eContinue;
}

const eStateStatus StateStack::UpdateState(const CU::Time& aDeltaTime)
{
	if (myStates.GetLast()->GetLetThroughUpdate() == true)
	{
		UpdateStateAtIndex(aDeltaTime, myStates.Size() - 2);
	}

	return myStates.GetLast()->Update(aDeltaTime);
}

void StateStack::UpdateStateAtIndex(const CU::Time& aDeltaTime, const short aIndex)
{
	if (aIndex <= 0) return;

	if (myStates[aIndex]->GetLetThroughUpdate() == true && aIndex > 0)
	{
		UpdateStateAtIndex(aDeltaTime, aIndex - 1);
	}

	myStates[aIndex]->Update(aDeltaTime);
}

void StateStack::RenderStateAtIndex(const short aIndex)
{
	if (myStates[aIndex]->GetLetThroughRender() == true && aIndex > 0)
	{
		RenderStateAtIndex(aIndex - 1);
	}

	myStates[aIndex]->Render();
}

bool StateStack::Update(const CU::Time& aDeltaTime)
{
	if (myStates.Size() > 0)
	{
		if (myShouldUpdate == true)
		{
			if (UpdateState(aDeltaTime) == eStateStatus::ePop)
			{
				Pop();
			}

			if (mySwapStateFunction != nullptr)
			{
				mySwapStateFunction();
				mySwapStateFunction = nullptr;
			}

			if (myStateToSwapTo != nullptr)
			{
				Pop();
				PushState(myStateToSwapTo);
				myStateToSwapTo = nullptr;
			}
		
		}

		return STATESTACK_CONTINUE;
	}

	return STATESTACK_QUIT;
}

void StateStack::Render()
{
	if (myStates.Size() > 0)
	{
		if (myStates.GetLast()->GetLetThroughRender() == true)
		{
			RenderStateAtIndex(myStates.Size() - 2);
		}

		myStates.GetLast()->Render();
	}
}

void StateStack::Pop()
{
	myStates.GetLast()->OnExit(false);
	bool letThroughRender = myStates.GetLast()->GetLetThroughRender();
	delete myStates.Pop();

	if (myStates.Size() > 0)
	{
		myStates.GetLast()->OnEnter(letThroughRender);
	}
}

void StateStack::Clear()
{
	while (myStates.Size() > 0)
	{
		myStates.GetLast()->OnExit(false);
		delete myStates.Pop();
	}
}

void StateStack::AddSwapStateFunction(const std::function<void(void)>& aSwapStateCunction)
{
	mySwapStateFunction = aSwapStateCunction;
}

void StateStack::SwapState(State* aState)
{
	myStateToSwapTo = aState;
}

