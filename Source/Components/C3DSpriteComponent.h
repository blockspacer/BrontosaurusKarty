#pragma once
#include "Component.h"
#include "Engine.h"
#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/vector4.h"
class CSprite;
class CScene;

class C3DSpriteComponent : public CComponent
{
public:

	C3DSpriteComponent(CScene& aScene, const std::string& aPath, const CU::Vector2f& aSize = CU::Vector2f::One, const CU::Vector2f& aPivot = CU::Vector2f::Zero,
		const CU::Vector4f& aRect = { 0.f,0.f,1.f,1.f }, const CU::Vector4f& aColor = CU::Vector4f::One, int aOwnerIndex = -1);
	~C3DSpriteComponent();
	void Destroy() override;
	void Render();
protected:
	CU::Vector2f mySize;
	CU::Vector2f myPivot;
	CU::Vector4f myRect;
	CU::Vector4f myColor;
	CSprite* mySprite;
	CScene& myScene;
	int myOwnerIndex;
};

