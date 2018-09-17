// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TetrisPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TETRIS3D_API ATetrisPlayerState : public APlayerState
{
	GENERATED_BODY()

	float RotationZ = 0;
public:
	void AddRotationZ(float amount);
	float GetRotationZ();
};
