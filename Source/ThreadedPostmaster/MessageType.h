#pragma once

enum class eMessageType
{
	eCharPressed,
	eWindowSizeChanged,
	eStateStackMessage,
	eFocusChanged,
	eDrawCallsThisFrame,
	eDestroyComponent,
	eConsoleCalledUpon,
	eDroppedFile,
	eChangeLevel,
	eQuitGame,
	eGameEventMessage,
	eNetworkMessage,
	ePrint,
	eDebugInfo,
	eAddToCheckPointRespawn,
	eSetNewCheckPoint,
	eResetToCheckPointMessage,
	eCreateExplosionMessage,
	eDeactivateExplosion,
	eRevivePlayer,
	eSpawnWave,
	eAddEnemyToWave,
	ePlayerEnterArena,
	eSetVibration,
	eStopVibration,
	eGetAvailableController,
	ePlayerFinished,
	eAIFInished,
	eRaceOver,
	eRaceStarted,
	eBlueShellWarning,
	eControllerInput,
	eRedShellWarning,
	ePlayerPassedGoal,
	eLength,
	
};

static const int MessageTypeLength = static_cast<int>(eMessageType::eLength);
