// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisStatics.h"
#include <Engine/StaticMesh.h>
#include <Engine/World.h>
#include <GameFramework/PlayerStart.h>
#include <Rules/TetrisGameState.h>
#include <EngineUtils.h>

int GetRotationQuadrant(float RotationZ)
{
	float Modded = FMath::Fmod(RotationZ, 360.0f);
	if (RotationZ < 0)
	{
		Modded = 360.0f - Modded;
	}
	if (Modded < 45.0f || Modded >= 315.0f)
	{
		return 3;
	}
	if (Modded >= 45.0f && Modded < 135.0f)
	{
		return 0;
	}
	if (Modded >= 135.0f && Modded < 225.0f)
	{
		return 1;
	}
	return 2;
}

FVector GetBlockWorldLocation(UWorld* World, FIntVector BlockLocation)
{
	ATetrisGameState* GameState = Cast<ATetrisGameState>(World->GetGameState());
	if (ensure(GameState))
	{
		return GetBlockWorldLocation(*TObjectIterator<APlayerStart>(), GameState->GetBlockSize(), BlockLocation);
	}
	else
	{
		return FVector();
	}
}

FVector GetBlockWorldLocation(APlayerStart* PlayerStart, float BlockSize, FIntVector BlockLocation)
{
	FVector LocalLocation = FVector(BlockLocation) * BlockSize;
	return PlayerStart->GetActorTransform().TransformPosition(LocalLocation);
}

void SetupBlockSceneComponent(class UStaticMeshComponent* SpawnedMesh, float BlockSize, FIntVector BlockLocation)
{
	FBox BoundingBox = SpawnedMesh->GetStaticMesh()->GetBoundingBox();
	FVector Bounds = BoundingBox.GetSize();
	FVector Scale = Bounds * (1.0f / BlockSize);
	SpawnedMesh->SetRelativeScale3D(Scale);
	SpawnedMesh->SetRelativeLocation(FVector(BlockLocation) * BlockSize - BoundingBox.GetCenter());
}

void SetupBlockSceneComponent(class UStaticMeshComponent* SpawnedMesh, UWorld* World, FIntVector BlockLocation)
{
	ATetrisGameState* GameState = Cast<ATetrisGameState>(World->GetGameState());
	if (ensure(GameState))
	{
		SetupBlockSceneComponent(SpawnedMesh, GameState->GetBlockSize(), BlockLocation);
	}
}