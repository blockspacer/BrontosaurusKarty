#include "stdafx.h"
#include "SpriteManager.h"
#include "Sprite.h"


CSpriteManager::CSpriteManager()
	: mySprites(ourMaxNumberOfSprites)
{
}


CSpriteManager::~CSpriteManager()
{
	mySpriteIDs.clear();
	mySprites.DeleteAll();
}

CSprite* CSpriteManager::CreateSprite(const std::string& aSpritePath)
{
	std::lock_guard<std::mutex> mutexGuard(myLoadMutex);
	//myLoadMutex.lock();

	if (mySpriteIDs.find(aSpritePath) == mySpriteIDs.end())
	{
		SpriteID spriteID = mySprites.Size();
		CSprite* newSprite = new CSprite();
		newSprite->Init(aSpritePath);
		mySprites.Add(newSprite);
		mySpriteIDs[aSpritePath] = spriteID;
	}

	SpriteID spriteID = mySpriteIDs[aSpritePath];
	//myLoadMutex.unlock();
	return mySprites[spriteID];
}

