#include "stdafx.h"
#include "Client.h"
#include "../CommonUtilities/DL_Debug.h"
#include <iostream>
#include "../TShared/PackageType.h"
#include "../TShared/NetworkMessage_Ping.h"
#include "../TShared/NetworkMessage_PingResponse.h"
#include "../TShared/NetworkMesssage_Connect.h"
#include "../TShared/NetworkMessage_ConectResponse.h"
#include "../CommonUtilities/CommonUtilities.h"
#include "../TShared/NetworkMessage_ChatMessage.h"
#include "ClientMessageManager.h"
#include "../PostMaster/EMessageReturn.h"
#include "../PostMaster/Message.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../CommonUtilities/ThreadPool.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../ThreadedPostmaster/ResetToCheckPointMessage.h"
#include "ServerReadyMessage.h"
#include "../CommonUtilities/ThreadNamer.h"


#include "../Components/GameObject.h"
#include "../Components/ComponentMessage.h"
#include "../ThreadedPostmaster/ConetctMessage.h"
#include "../ThreadedPostmaster/ConectedMessage.h"
#include "../TShared/NetworkMessage_LoadLevel.h"
#include "../ThreadedPostmaster/LoadLevelMessage.h"
#include "../ThreadedPostmaster/SetClientIDMessage.h"
#include "../TShared/NetworkMessage_ServerReady.h"
#include "../ThreadedPostmaster/NetworkDebugInfo.h"
#include "../ThreadedPostmaster/GameEventMessage.h"
#include "../Game/GameEventMessenger.h"
#include "../CommonUtilities/StringHelper.h"
#include "../TShared/NetworkMessage_Disconected.h"
#include "../TShared/NetworkMessage_AnimationStart.h"


CClient::CClient() : myMainTimer(0), myState(eClientState::DISCONECTED), myId(0), myServerIp(""), myServerPingTime(0), myServerIsPinged(false), myPlayerPositionUpdated(false), myRoundTripTime(0), myCurrentTime(0), myPositionWaitTime(0)
{
	myCanUpdateEnemytransfromation = true;
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eNetworkMessage);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eChangeLevel);
	CClientMessageManager::CreateInstance(*this);

	myIsRunning = true;
	myCanQuit = false;
}


CClient::~CClient()
{
	myIsRunning = false;
	while (myCanQuit == false)
	{
		continue;
	}
	CClientMessageManager::DestroyInstance();
}

bool CClient::StartClient()
{
	myNetworkWrapper.Init(0, CClientMessageManager::GetInstance());
	myMainTimer = myTimerManager.CreateTimer();
	CU::Work work(std::bind(&CClient::Update, this));
	work.SetName("ClientUpdate");
	std::function<bool(void)> condition;
	condition = std::bind(&CClient::IsRunning, this);
	//condition = []()->bool{return true; };
	work.AddLoopCondition(condition);
	std::function<void()> callback = [this]() {myCanQuit = true;  };
	work.SetFinishedCallback(callback);
	CEngine::GetInstance()->GetThreadPool()->AddWork(work);
	return true;
}

void CClient::Disconect()
{
	myState = eClientState::DISCONECTED;
	std::cout << "Disconected from server";
	myServerIsPinged = false;
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"Disconected From Server"));
	//myChat.StopChat();
}

void CClient::UpdatePing(const CU::Time& aTime)
{
	if (myServerIsPinged && myState == eClientState::CONECTED)
	{
		myServerPingTime += aTime.GetSeconds();

		if (myServerPingTime.GetSeconds() >= 20)
		{
			std::cout << "Server is not responding" << std::endl;
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(L"Server Not Responding"));
			Disconect();
		}
	}
}

void CClient::Ping()
{
	if (myState == eClientState::CONECTED && myServerIsPinged == false)
	{
		CNetworkMessage_Ping* tempMessagePing = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_Ping>("__Server");

		myNetworkWrapper.Send(tempMessagePing, myServerIp.c_str(), SERVER_PORT);

		myServerIsPinged = true;
		myServerPingTime = 0;
	}
}

void CClient::Update()
{

		myTimerManager.UpdateTimers();
		myCurrentTime += myTimerManager.GetTimer(myMainTimer).GetDeltaTime().GetSeconds();
		const CU::Time deltaTime = myTimerManager.GetTimer(myMainTimer).GetDeltaTime();
		
		myPositionWaitTime += deltaTime;
		myServerPingTime += deltaTime;
		CNetworkMessage* currentMessage = myNetworkWrapper.Recieve(nullptr, nullptr);

		while (currentMessage->GetHeader().myPackageType != ePackageType::eZero)
		{
			switch ((currentMessage->GetHeader().myPackageType))
			{
			case ePackageType::eConnectResponse:
				if (myState == eClientState::CONECTING)
				{
					CNetworkMessage_ConectResponse* conectResponseMessage = currentMessage->CastTo<CNetworkMessage_ConectResponse>();
					myId = conectResponseMessage->myClientId;
					myState = eClientState::CONECTED;

					std::cout << "Conected to server got id:" << myId << std::endl;
					Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CConectedMessage(myId));
				}
				break;
			case ePackageType::ePing:
				if (myState == eClientState::CONECTED)
				{
					//DL_PRINT("CLIENT:Ping");

					CNetworkMessage_PingResponse* newMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_PingResponse>("__Server");
					myNetworkWrapper.Send(newMessage, myServerIp.c_str(), SERVER_PORT);
				}
				break;
			case ePackageType::ePingResponse:
			{
				//DL_PRINT("CLIENT:PingRespons");
				myRoundTripTime = myServerPingTime.GetMilliseconds();
				myServerPingTime = 0;
				myServerIsPinged = false;
			}
			break;
			case ePackageType::eChat:
			{
				CNetworkMessage_ChatMessage *chatMessage = currentMessage->CastTo<CNetworkMessage_ChatMessage>();
				std::cout << chatMessage->myChatMessage << std::endl;
			}
			break;
			case ePackageType::eServerReady:
			{
				CNetworkMessage_ServerReady* serverReady = currentMessage->CastTo<CNetworkMessage_ServerReady>();
				myCanUpdateEnemytransfromation = true;
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CServerReadyMessage(serverReady->GetNumberOfPlayers()));
			}
			break;
			case ePackageType::eLoadLevel:
			{
				CNetworkMessage_LoadLevel *loadLevelMessage = currentMessage->CastTo<CNetworkMessage_LoadLevel>();
				myCanUpdateEnemytransfromation = false;
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CLoadLevelMessage(loadLevelMessage->myLevelIndex));
			}
			break;
			case ePackageType::eConnect:
			{
				CNetworkMessage_Connect* conectMessage = currentMessage->CastTo<CNetworkMessage_Connect>();
				std::wstring string;
				string += L"Player ";
				string += CU::StringToWString(conectMessage->myClientName);
				string += L" has conected!";
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(string));
			}
			break;
			case ePackageType::eDisconected:
			{
				CNetworkMessage_Disconected* disconectedMessage = currentMessage->CastTo<CNetworkMessage_Disconected>();
				std::wstring string;
				string += L"Player ";
				string += CU::StringToWString(disconectedMessage->GetClientName());
				string += L" has disconected!";
				Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CGameEventMessage(string));
			}
			break;
			case ePackageType::eAnimationStart:
			{
				CNetworkMessage_AnimationStart* animationMessage = currentMessage->CastTo<CNetworkMessage_AnimationStart>();
			}
			break;
			case ePackageType::eStartCountdown:
			{
				int br = 0;
			}
			break;

			case ePackageType::eZero:
			case ePackageType::eSize:
			default: break;
			}
			currentMessage = myNetworkWrapper.Recieve(nullptr, nullptr);
		}

		UpdatePing(myTimerManager.GetTimer(myMainTimer).GetDeltaTime());
		


		if (myCurrentTime > 1.f)
		{
			Ping();
			myCurrentTime = 0.f;

			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CNetworkDebugInfo(myNetworkWrapper.GetAndClearDataSent(), myRoundTripTime));
		}
}

void CClient::Send(CNetworkMessage* aNetworkMessage)
{
	myNetworkWrapper.Send(aNetworkMessage, myServerIp.c_str(), SERVER_PORT);
}

bool CClient::Connect(const char* anIp, std::string aClientName)
{
	myName = aClientName;
	myState = eClientState::CONECTING;

	myServerIp = anIp;

	CNetworkMessage_Connect* message = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_Connect>("__Server");
	message->myClientName = aClientName;

	myNetworkWrapper.Send(message, anIp, SERVER_PORT);

	DL_PRINT("wait Connecting");
	while (myState == eClientState::CONECTING)
	{
		std::this_thread::yield();
	}

	return  true;
}

short CClient::GetID()
{
	return myId;
}

eMessageReturn CClient::DoEvent(const CSendNetworkMessageMessage& aSendNetworkMessageMessage)
{
	Send(aSendNetworkMessageMessage.UnpackHolder());
	return eMessageReturn::eContinue;
}

eMessageReturn CClient::DoEvent(const CConectMessage& aConectMessage)
{
	Connect(aConectMessage.myIP.c_str(), aConectMessage.myName);
	return eMessageReturn::eContinue;
}

eMessageReturn CClient::DoEvent(const CChangeLevel& aChangeLevelMessage)
{
	myCanUpdateEnemytransfromation = false;
	return eMessageReturn::eContinue;
}

bool CClient::IsRunning()
{
	return myIsRunning;
}
