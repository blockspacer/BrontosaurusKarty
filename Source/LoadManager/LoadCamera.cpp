#include "stdafx.h"
#include "LoadCamera.h"
#include "CameraComponent.h"

int LoadCamera(KLoader::SLoadedComponentData /*someData*/)
{
	GET_LOADMANAGER(loadManager);

	CU::Camera& playerCamera = loadManager.GetCurrentScene().GetRenderCamera(CScene::eCameraType::ePlayerOneCamera).GetCamera();

	CCameraComponent* cameraComponent = new CCameraComponent();
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(playerCamera);

	//loadManager.GetCurrentPLaystate().SetCameraComponent(cameraComponent);

	return cameraComponent->GetId();
}
