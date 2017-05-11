#pragma once

class CPollingStation
{
public:
	static void Create();
	static void Destroy();
	static CPollingStation* GetInstance()			{ return ourInstance; }
	static unsigned char GetStartCountdownTime()	{ return myStartCountdownTime; }
private:
	CPollingStation();
	~CPollingStation();

	void SetStartCountDownTime(unsigned char aTime) { myStartCountdownTime = aTime; }

private:
	static CPollingStation* ourInstance;
	static unsigned char myStartCountdownTime;
};

