#include "stdafx.h"
#include "KartSpawnPoint.h"


CKartSpawnPoint::CKartSpawnPoint()
{
}


CKartSpawnPoint::~CKartSpawnPoint()
{
}

void CKartSpawnPoint::SetTransformation(CU::Matrix44f aTransformaion)
{
	mySpawnTransformaion = aTransformaion;
}
