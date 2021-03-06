#include "stdafx.h"
#include "CameraComponent.h"
#include "../CommonUtilities/Camera.h"
#include "../Audio/AudioInterface.h"
#include "../CommonUtilities/PICarlApproved.h"
#include "../CommonUtilities/JsonValue.h"

//float pascalTriangle(float a, float b);
//
//float generalSmoothStep(float N, float x)   //Generalized smoothstep
//{
//	//x must be between 0 and 1
//	float result = 0;
//	for (float n = 0; n <= N; n+=1.f)
//	{
//		result += (pascalTriangle(-N - 1, n) * pascalTriangle(2 * N + 1, N - n) * powf(x, N + n + 1));
//	}
//	return result;
//}
//
//float pascalTriangle(float a,float b)
//{
//	//Pascal triangle for replacement with binomial coefficient because a factorial can't be used with negative numbers
//	float result = 1;
//	for (float i = 0; i < b; i += 1.f)
//	{
//		result *= (a - i) / (i + 1);
//	}
//	return result;
//}

CCameraComponent::CCameraComponent() : CCameraComponent(0)
{
}

CCameraComponent::CCameraComponent(unsigned int aPlayerCount)
	: myCamera(nullptr)
	, myInterpolatingSpeed(35.1f)
	, myAccumulatedRotation(0.f)
	, myUnlocked(false)
{
	myType = eComponentType::eCamera;

	CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse("Json/Camera.json");
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue cameraSettings = levelsFile.at("Camera");
	CU::CJsonValue setting;
	const std::string countKey = std::to_string(aPlayerCount);
	if(cameraSettings.HasKey(countKey) == true)
	{
		setting = cameraSettings.at(countKey);
	}
	else
	{
		setting = cameraSettings.at("Default");
	}

	float radians = setting.at("CameraRotationX").GetFloat() * PI/180;


	float rotationAngle = radians;
	myKartOffset.RotateAroundAxis(rotationAngle, CU::Axees::X);
	myKartOffset.Move(CU::Vector3f(0.0f, 0.0f, setting.at("CameraOffsetZ").GetFloat()));
	myKartOffset.GetPosition() += CU::Vector3f(0.0f, setting.at("CameraOffsetY").GetFloat(), .0f);
}

CCameraComponent::~CCameraComponent()
{
}

void CCameraComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (myCamera == nullptr)
	{
		return;
	}

	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponentTypeAdded == eComponentType::eCamera)
		{
			GetParent()->SetName("Camera");
		}
		break;
	case eComponentMessageType::eObjectDone:
		myCamera->SetTransformation(GetParent()->GetToWorldTransform());
	case eComponentMessageType::eMoving:
		if (myUnlocked == false)
		{
			myCameraInterpolateTowardsMatrix = myKartOffset * GetParent()->GetToWorldTransform();
			
			//Audio::CAudioInterface::GetInstance()->SetListenerPosition(transformation); hasta la xp vista bebe
		}
		break;
	case eComponentMessageType::eSetCameraRotation:
		{
			GetParent()->GetLocalTransform().SetRotation(*reinterpret_cast<CU::Matrix33f*>(aMessageData.myVoidPointer));
			myCameraInterpolateTowardsMatrix = myKartOffset * GetParent()->GetToWorldTransform();
			myCamera->SetTransformation(myCameraInterpolateTowardsMatrix);
		}
		break;
	}
}

bool CCameraComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eHasCameraComponent:
		aQuestionData.myBool = true;
		return true;
	}

	return false;
}

void CCameraComponent::SetCamera(CU::Camera& aCamera)
{
	myCamera = &aCamera;
}

void CCameraComponent::Update(float aDeltaTime)
{
	CU::Matrix44f& cameraTransform = myCamera->GetTransformation();

	cameraTransform.Lerp(myCameraInterpolateTowardsMatrix, myInterpolatingSpeed * aDeltaTime);
	cameraTransform.LerpPosition(myCameraInterpolateTowardsMatrix.myPosition, myInterpolatingSpeed * aDeltaTime);
}
