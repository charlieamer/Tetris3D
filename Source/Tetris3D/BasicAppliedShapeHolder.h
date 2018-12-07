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

	// Called when shape is registered in the level
	UFUNCTION()
	void ShapeApplied(class ABaseShape* Shape);

	// Called when shape is finished dropping to the floor (could be called immediately if space is not pressed)
	UFUNCTION()
	void ShapeDone(class ABaseShape* Shape);

	// Called when user fills a level
	UFUNCTION()
	void OnLevelsDestroyed(TArray<int> Levels);

	UPROPERTY(EditInstanceOnly)
	UStaticMesh* MeshToSpawn;

	UPROPERTY(EditInstanceOnly)
	TSubclassOf<AActor> ActorWhenBlockDestroyed;

	TArray<int> LevelDestroyQueue;

	void AddCube(FIntVector Location);
public:
	ABasicAppliedShapeHolder();
};
