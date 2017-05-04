#pragma once
#include "..\CommonUtilities\matrix44.h"

class CKartSpawnPoint
{
public:
	CKartSpawnPoint();
	~CKartSpawnPoint();
	void SetTransformation(CU::Matrix44f aTransformaion);

private:
	unsigned char myID;
	CU::Matrix44f mySpawnTransformaion;
};

