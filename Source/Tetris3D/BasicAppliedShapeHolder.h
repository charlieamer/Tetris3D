// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicAppliedShapeHolder.generated.h"

UCLASS()
class TETRIS3D_API ABasicAppliedShapeHolder : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ShapeApplied(class ABaseShape* Shape);

	UFUNCTION()
	void ShapeDone(class ABaseShape* Shape);

	UPROPERTY(EditInstanceOnly)
	UStaticMesh* MeshToSpawn;

public:
	ABasicAppliedShapeHolder();
};
