#pragma once
#include "KartControllerComponent.h"

class CColliderComponent;
struct SNavigationPoint;

namespace CU
{
	template<typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;

	template<typename TYPE>
	class Matrix44;
	using Matrix44f = Matrix44<float>;

	template<typename TYPE>
	class Matrix33;
	using Matrix33f = Matrix33<float>;

	template<typename TYPE>
	class Vector3;
	using Vector3f = Vector3<float>;

	template<typename TYPE>
	class Vector2;
	using Vector2f = Vector2<float>;
}

enum class eComponentQuestionType
{
	eHasCameraComponent,
	eGetCameraLookat,
	eGetCameraPosition,
	eGetCameraObject,
	eGetCollisionShape,
	eMovePhysicsController,
	ePhysicsControllerConstraints,
	eHaveTriggerCollision,
	eHitPosition,
	eLastHitNormal,
	eLastHitPosition,
	eGetRotatorObject,
	eGetIsGrounded,
	eGetSplineDirection,
	eGetSplineWithIndex,
	eGetLapIndex,
	eGetHoldItemType,
	eGetTerrainModifier,
	eLength,
};
struct SComponentQuestionData
{
	SComponentQuestionData();

	union
	{
		CU::Vector4f myVector4f;
		CU::Vector3f myVector3f;
		CColliderComponent* myCollider;

		int myInt;
		float myFloat;

		bool myBool;
		char myChar;

		const char* myString;
		CGameObject* myGameObject;
		const SNavigationPoint* myNavigationPoint;
	};

};

inline SComponentQuestionData::SComponentQuestionData(): myVector3f()
{
}
