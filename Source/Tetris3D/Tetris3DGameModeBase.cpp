// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris3DGameModeBase.h"
#include "Rules/TetrisGameState.h"
#include "Rules/TetrisPlayerController.h"
#include "TetrisPlayerState.h"

void ATetris3DGameModeBase::InitGameState()
{
	Super::InitGameState();
	GetTetrisState()->Initialize(10, 10, 10, 100, 1);
}

ATetris3DGameModeBase::ATetris3DGameModeBase() : Super()
{
	GameStateClass = ATetrisGameState::StaticClass();
	PlayerControllerClass = ATetrisPlayerController::StaticClass();
	PlayerStateClass = ATetrisPlayerState::StaticClass();
}

ATetrisGameState* ATetris3DGameModeBase::GetTetrisState()
{
	return GetGameState<ATetrisGameState>();
}
