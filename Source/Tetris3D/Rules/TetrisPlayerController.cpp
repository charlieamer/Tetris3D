// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisPlayerController.h"
#include "TetrisCamera.h"
#include <GameFramework/PlayerStart.h>
#include <Rules/TetrisGameState.h>

void ATetrisPlayerController::BeginPlay()
{
	Super::BeginPlay();
	APlayerStart* StartPoint = *TObjectIterator<APlayerStart>();
	ATetrisGameState* GameState = Cast<ATetrisGameState>(GetWorld()->GetGameState());
	if (ensureMsgf(CameraController_BP, TEXT("Camera controller missing")) &&
		ensureMsgf(StartPoint, TEXT("No start points in the level")) &&
		ensureMsgf(GameState, TEXT("Game state must be of ATetrisGameState")))
	{
		FVector Transformed(
			GameState->GetSizeX() / 2 * GameState->GetBlockSize(),
			GameState->GetSizeY() / 2 * GameState->GetBlockSize(),
			GameState->GetSizeZ() / 3 * GameState->GetBlockSize()
		);
		TetrisPlayerCamera = GetWorld()->SpawnActor<ATetrisCamera>(
			CameraController_BP,
			StartPoint->GetTransform().TransformPosition(Transformed),
			StartPoint->GetActorRotation()
		);
		TetrisPlayerCamera->SetController(this);
		bAutoManageActiveCameraTarget = false;
		SetViewTargetWithBlend(TetrisPlayerCamera);
	}
}
