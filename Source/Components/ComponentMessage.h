#pragma once

class ICollider;
class CComponent;
class CGameObject;
struct SBoostData;
enum class ePlayerControls;

namespace std
{
	template<class _Fty>
	class function;
}

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
enum class eComponentType : unsigned char;


enum class eComponentMessageType
{
	eAddComponent,
	ePlaySound,
	eMoving,
	eSetVisibility,
	eSetIsColliderActive,
	eObjectDone,
	eAddMessageSubscription,
	eSetHighlight,
	eTurnOnThePointLight,
	eTurnOffThePointLight,
	eSetPointLightIntensity,
	eSetPointLightRange,
	eSetPointLightColor,
	eSetPointLightToLastState,
	eKeyPressed,
	eKeyReleased,
	eOnTriggerEnter,
	eOnTriggerExit,
	eOnCollisionEnter,
	eOnCollisionExit,
	eSetControllerPosition,
	eActivate,
	eDeactivate,
	eActivateEmitter,
	eDeactivateEmitter,
	eAddForce,
	eSetLastHitNormal,
	eSetLastHitPosition,
	eAccelerate,
	eStopAcceleration,
	eDecelerate,
	eStopDeceleration,
	eTurnKart,
	eStopTurnKart,
	eGiveBoost,
	eSetBoost,
	eGiveItem,
	eUseItem,
	eRotateTowards,
	eUpdate,
	eKill,
	eGotHit,
	eRespawn,
	eDoDriftBobbing,
	eCancelDriftBobbing,
	eMakeInvurnable,
	eSpinKart,
	eEnteredGoal,
	eTurnOffHazard,
	eReInitRedShell,
	eReInitBlueShell,
	ePassedASpline,
	eAITakeOver,
	eSetCameraRotation,
	eResetExplosion,
	eHazzardCollide,
	eFinishedRace,
	eLength,
};

struct SComponentMessageData
{
	SComponentMessageData() : myVoidPointer(nullptr) {}

	union
	{
		void* myVoidPointer;
		struct //don't do this please name structs
		{
			CComponent* myComponent;
			eComponentType myComponentTypeAdded;
		};
		
		struct //don't do this please name structs 
		{
			bool myPhysicsUpdated;
			bool myBool;
		};
		CGameObject* myGameObject;
		struct SComponentMessageCallback* myComponentMessageCallback;
		ePlayerControls myPlayerControl;
		int myInt;
		unsigned short myUShort;
		unsigned char myUChar;
		const char* myString;
		float myFloat;
		CU::Vector2f myVector2f;
		CU::Vector3f myVector3f;
		CU::Vector4f myVector4f;

		ICollider* myCollider;
		const SBoostData* myBoostData;

		const std::function<void(void)>* myVoidFunction;
	};
};

static_assert(sizeof(SComponentMessageData) <= sizeof(CU::Vector4f), "Don't put so big things in the component message data :((");
