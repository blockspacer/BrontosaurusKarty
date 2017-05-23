#pragma once
#include "CollisionLayers.h"

namespace physx
{
	class PxShape;
}

namespace Physics
{

	//enum ECollisionLayer;

	class CShape
	{
		friend class CPhysics;
		friend class CPhysicsActor;

	public:
		CShape(physx::PxShape* aShape);
		CShape(const CShape& aShape);
		~CShape();

		void SetCollisionLayers(const ECollisionLayer& aLayer);
		void SetCollisionLayers(const ECollisionLayer& aLayer, const unsigned int aLayerToCollideWith);
		void SetObjectId(unsigned int aI);
		physx::PxShape* GetShape();
	private:
		physx::PxShape* myShape;


#ifdef _DEBUG
		ECollisionLayer myLayer;
		ECollisionLayer myCollideAgainst;
#endif


	};
}
