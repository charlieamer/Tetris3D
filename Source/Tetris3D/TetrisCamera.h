// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "TetrisCamera.generated.h"

class AController;
class ATetrisPlayerState;

/**
 * 
 */
UCLASS()
class TETRIS3D_API ATetrisCamera : public ACameraActor
{
	GENERATED_BODY()
	ATetrisPlayerState* PlayerState = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float Distance = 1000;
	UPROPERTY(EditDefaultsOnly)
	float Elevation = 45;


	void UpdateCamera();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinDistance = 300;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxDistance = 1300;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinElevation = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxElevation = 70;

	UFUNCTION(BlueprintCallable)
	void AddDistance(float amount);

	UFUNCTION(BlueprintCallable)
	void AddRotation(float amount);

	UFUNCTION(BlueprintCallable)
	void AddElevation(float amount);

public:
	void SetController(AController* ControllerToSet);
};
