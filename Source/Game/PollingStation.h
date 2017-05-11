#pragma once

class CPollingStation
{
friend class CClient;

public:
	static void Create();
	static void Destroy();
	static CPollingStation* GetInstance()			{ return ourInstance; }
	unsigned char GetStartCountdownTime()	{ return myStartCountdownTime; }
private:
	CPollingStation();
	~CPollingStation();

	inline void SetStartCountDownTime(unsigned char aTime) { myStartCountdownTime = aTime; }

private:
	static CPollingStation* ourInstance;
	unsigned char myStartCountdownTime;
};

