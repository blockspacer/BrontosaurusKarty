#pragma once

class CColliderComponent;

namespace CU
{
	template<typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;
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

		bool myBool;
		char myChar;

		const char* myString;
		CGameObject* myGameObject;
	};
};

inline SComponentQuestionData::SComponentQuestionData(): myVector3f()
{
}
