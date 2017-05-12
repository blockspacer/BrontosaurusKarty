#pragma once

class CPollingStation
{
friend class CClient;

public:
	static void Create();
	static void Destroy();
	static CPollingStation* GetInstance()			{ return ourInstance; }
private:
	CPollingStation();
	~CPollingStation();

private:
	static CPollingStation* ourInstance;
};

