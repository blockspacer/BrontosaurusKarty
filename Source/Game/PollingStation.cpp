#include "stdafx.h"
#include "PollingStation.h"

CPollingStation* CPollingStation::ourInstance = nullptr;

void CPollingStation::Create()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CPollingStation();
	}
	else
	{
		DL_ASSERT("CPollingStation already created");
	}
}

void CPollingStation::Destroy()
{
	SAFE_DELETE(ourInstance);
}

CPollingStation::CPollingStation()
{
	myStartCountdownTime = 3;
}

CPollingStation::~CPollingStation()
{
}