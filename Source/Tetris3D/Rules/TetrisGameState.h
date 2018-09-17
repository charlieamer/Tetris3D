// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TetrisGameState.generated.h"

/**
 * 
 */
UCLASS()
class TETRIS3D_API ATetrisGameState : public AGameStateBase
{
	GENERATED_BODY()
	TSharedPtr<bool> Fields;
	int SizeX, SizeY, SizeZ;
	int GetIndex(int X, int Y, int Z);
	float BlockSize;
	float DropSpeed;
public:
	void Initialize(int SizeX, int SizeY, int SizeZ, float BlockSize, float DropSpeed);
	bool IsValidPosition(int X, int Y, int Z);
	bool IsValidPosition(const FIntVector& Position);
	float GetBlockSize();
	int GetSizeX();
	int GetSizeY();
	int GetSizeZ();
	float GetDropSpeed();
};
