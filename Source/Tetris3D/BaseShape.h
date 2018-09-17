// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseShape.generated.h"

UCLASS(Blueprintable)
class TETRIS3D_API ABaseShape : public APawn
{
	GENERATED_BODY()

	FIntVector Position = FIntVector(0, 0, 0);
	FVector WorldLocation;

public:
	// Sets default values for this pawn's properties
	ABaseShape();

	const TArray<FIntVector>& GetShape();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float BlockSize;

	UFUNCTION()
	void MoveShapeXP();
	UFUNCTION()
	void MoveShapeXN();
	UFUNCTION()
	void MoveShapeYP();
	UFUNCTION()
	void MoveShapeYN();

	void TryMoveShape(FIntVector Amount);

	FVector UpdateWorldLocation();

	bool IsValidPosition();

	UPROPERTY(EditDefaultsOnly)
	TArray<FIntVector> Shape;

	class APlayerStart* ShapeStart;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* MeshToSpawn;

	FVector GetWorldLocation();
};
