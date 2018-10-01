// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris3DGameModeBase.h"
#include "Rules/TetrisGameState.h"
#include "Rules/TetrisPlayerController.h"
#include <GameFramework/Controller.h>
#include "BaseShape.h"
#include "TetrisPlayerState.h"

void ATetris3DGameModeBase::SpawnNewShape(TArray<AController*> ShapeControllers)
{
	if (ensure(PossibleShapes.Num() > 0))
	{
		const TSubclassOf<ABaseShape> ShapeToSpawn = PossibleShapes[FMath::RandRange(0, PossibleShapes.Num() - 1)];
		ABaseShape* SpawnedShape = GetWorld()->SpawnActor<ABaseShape>(ShapeToSpawn);
		for (AController* ShapeController : ShapeControllers)
		{
			ShapeController->Possess(SpawnedShape);
		}
	}
}

void ATetris3DGameModeBase::SpawnNewShape(class AController* ShapeController)
{
	TArray<AController*> ShapeControllers;
	ShapeControllers.Add(ShapeController);
	SpawnNewShape(ShapeControllers);
}

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

void ATetris3DGameModeBase::ApplyShape(ABaseShape* Shape)
{
	for (const auto& Vec : Shape->GetShape())
	{
		GetTetrisState()->SetBlockAtPosition(Vec + Shape->GetPosition());
	}
	SpawnNewShape(Shape->GetController());
}
