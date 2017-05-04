#pragma once
#include "Component.h"
class CKartControllerComponent : public CComponent
{
public:
	CKartControllerComponent();
	~CKartControllerComponent();

	void MoveRight();
	void MoveLeft();
	void MoveFoward();
	void MoveBackWards();

private:
};

