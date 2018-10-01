// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseShape.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShapeMovedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShapeInitializedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShapeRotatedDelegate, FVector, Axis);

UCLASS(Blueprintable)
class TETRIS3D_API ABaseShape : public APawn
{
	GENERATED_BODY()

	FIntVector Position = FIntVector(0, 0, 0);
	float DropSpeed;
	float LastTimeDropped;
	bool bWasApplied;

	UPROPERTY(EditDefaultsOnly)
	bool bIsShadow;

	UPROPERTY()
	TWeakObjectPtr<ABaseShape> ParentShape;

public:
	// Sets default values for this pawn's properties
	ABaseShape();

	const TArray<FIntVector>& GetShape();

	UFUNCTION(BlueprintCallable)
	void SetParentShape(ABaseShape* InParentShape);

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
	UFUNCTION()
	void RotateShapeXP();
	UFUNCTION()
	void RotateShapeYP();
	UFUNCTION()
	void RotateShapeZP();
	UFUNCTION()
	void RotateShapeXN();
	UFUNCTION()
	void RotateShapeYN();
	UFUNCTION()
	void RotateShapeZN();

	void InitializeShapeMeshes();

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseShape> ShadowBaseShapeToSpawn;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryMoveShape(FIntVector Amount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryRotateShape(FIntVector Axis, int Amount);

	bool Server_TryMoveShape_Validate(FIntVector Amount);
	bool Server_TryRotateShape_Validate(FIntVector Axis, int Amount);
	void DoRotateShape(FIntVector Axis, int Amount);

	bool TryMoveShape(FIntVector Amount);

	bool IsValidPosition();

	UPROPERTY(EditAnywhere)
	TArray<FIntVector> Shape;

	UPROPERTY()
	TWeakObjectPtr<class APlayerStart> ShapeStart;

	void SetStartingPosition();

	void SetPositionAsShadow();

	void DropOneLevel();

	void ApplyShape();

	class ATetrisGameState* GameState;

	UFUNCTION()
	void ParentShapeMoved();
	UFUNCTION()
	void ParentShapeRotated(FVector Axis);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* MeshToSpawn;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetWorldLocation();

	FIntVector GetPosition();

	UPROPERTY(BlueprintAssignable)
	FShapeMovedDelegate ShapeMoved;

	UPROPERTY(BlueprintAssignable)
	FShapeInitializedDelegate ShapeInitialized;

	UPROPERTY(BlueprintAssignable)
	FShapeRotatedDelegate ShapeRotated;
};
