#pragma once

class CAnimationEvent
{
public:
	CAnimationEvent();
	~CAnimationEvent();

	bool Update(const float aDeltaTime);

private:
	float myTimer;
};
