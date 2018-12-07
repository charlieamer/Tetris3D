// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TetrisPlayerController.generated.h"

class ATetrisCamera;

/**
 * 
 */
UCLASS()
class TETRIS3D_API ATetrisPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ATetrisCamera* TetrisPlayerCamera;

public:
	UPROPERTY(EditAnywhere, Category = Setup)
	TSubclassOf<ATetrisCamera> CameraController_BP;
};
