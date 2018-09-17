// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraControlComponent.generated.h"

class AController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TETRIS3D_API UCameraControlComponent : public UActorComponent
{
	GENERATED_BODY()

	AController* controller;

public:	
	// Sets default values for this component's properties
	UCameraControlComponent();
	void SetController(AController* controller);
};
