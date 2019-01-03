// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseShape.h"
#include "Rules/TetrisGameState.h"
#include <Components/StaticMeshComponent.h>
#include <GameFramework/PlayerStart.h>
#include <Tetris3DGameModeBase.h>
#include <TetrisStatics.h>
#include <TetrisPlayerState.h>
#include <Engine/World.h>
#include <UObjectIterator.h>
#include <Materials/MaterialInstance.h>

// Sets default values
ABaseShape::ABaseShape() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

const TArray<FIntVector>& ABaseShape::GetShape()
{
	return Shape;
}

void ABaseShape::SetParentShape(ABaseShape* InParentShape)
{
	ParentShape = InParentShape;
	Shape = InParentShape->Shape;
	bIsShadow = true;
	InitializeShapeMeshes();
	SetPositionAsShadow();
	InParentShape->ShapeMoved.AddDynamic(this, &ABaseShape::ParentShapeMoved);
	InParentShape->ShapeRotated.AddDynamic(this, &ABaseShape::ParentShapeRotated);
	ShapeInitialized.Broadcast();
}

// Called when the game starts or when spawned
void ABaseShape::BeginPlay()
{
	Super::BeginPlay();
	if (!ensure(MeshToSpawn))
	{
		Destroy();
		return;
	}
	ShapeStart = *TObjectIterator<APlayerStart>();
	GameState = Cast<ATetrisGameState>(GetWorld()->GetGameState());
	BlockSize = GameState->GetBlockSize();
	LastTimeDropped = GetWorld()->GetTimeSeconds();

	if (!bIsShadow)
	{
		SetStartingPosition();
		InitializeShapeMeshes();
		ABaseShape* Shadow = GetWorld()->SpawnActor<ABaseShape>(ShadowBaseShapeToSpawn, GetTransform());
		Shadow->SetParentShape(this);
		ShapeInitialized.Broadcast();
	}
}

void ABaseShape::InitializeShapeMeshes()
{
	for (FIntVector ShapeBlock : Shape)
	{
		UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this);
		Mesh->RegisterComponent();
		Mesh->SetStaticMesh(MeshToSpawn);
		Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		SetupBlockSceneComponent(Mesh, BlockSize, ShapeBlock);
		Mesh->SetMaterial(0, MaterialToUse);
	}
}

void ABaseShape::SetStartingPosition()
{
	int HighestZ = -100;
	for (FIntVector ShapeBlock : Shape)
	{
		if (ShapeBlock.Z > HighestZ)
		{
			HighestZ = ShapeBlock.Z;
		}
	}
	if (ensure(HighestZ != -100))
	{
		Position = FIntVector(
			GameState->GetSizeX() / 2,
			GameState->GetSizeY() / 2,
			GameState->GetSizeZ() - HighestZ - 1
		);
		if (!IsValidPosition())
		{
			Destroy();
			UE_LOG(LogTemp, Error, TEXT("GAME OVER"));
		}
	}
}

void ABaseShape::SetPositionAsShadow()
{
	if (bIsShadow && (ParentShape == nullptr || !ParentShape->IsValidPosition()))
	{
		Destroy();
		return;
	}
	else if (ParentShape != nullptr)
	{
		Position = ParentShape->GetPosition();
	}
	for (; Position.Z >= 0 && IsValidPosition(); Position.Z--);
	Position.Z++;
}

void ABaseShape::MoveShapeXP()
{
	MoveShapeWithRotation(FIntVector(1, 0, 0));
}

void ABaseShape::MoveShapeXN()
{
	MoveShapeWithRotation(FIntVector(-1, 0, 0));
}

void ABaseShape::MoveShapeYP()
{
	MoveShapeWithRotation(FIntVector(0, -1, 0));
}

void ABaseShape::MoveShapeYN()
{
	MoveShapeWithRotation(FIntVector(0, 1, 0));
}

void ABaseShape::MoveShapeWithRotation(FIntVector Amount, float RotationZ)
{
	float Quadrant = GetRotationQuadrant(RotationZ);
	FVector RotatedAmount = FVector(Amount).RotateAngleAxis(90.0f * Quadrant, FVector(0, 0, 1));
	Server_TryMoveShape(FIntVector(
		FMath::RoundToInt(RotatedAmount.X),
		FMath::RoundToInt(RotatedAmount.Y),
		FMath::RoundToInt(RotatedAmount.Z)
	));
}

void ABaseShape::MoveShapeWithRotation(FIntVector Amount)
{
	if (ensure(GetController()))
	{
		MoveShapeWithRotation(Amount, Cast<ATetrisPlayerState>(GetController()->PlayerState)->GetRotationZ());
	}
}

void ABaseShape::RotateShapeXP()
{
	RotateShapeWithRotation(FIntVector(1, 0, 0), 1);
}

void ABaseShape::RotateShapeYP()
{
	RotateShapeWithRotation(FIntVector(0, 1, 0), 1);
}

void ABaseShape::RotateShapeZP()
{
	RotateShapeWithRotation(FIntVector(0, 0, 1), 1);
}

void ABaseShape::RotateShapeXN()
{
	RotateShapeWithRotation(FIntVector(1, 0, 0), -1);
}

void ABaseShape::RotateShapeYN()
{
	RotateShapeWithRotation(FIntVector(0, 1, 0), -1);
}

void ABaseShape::RotateShapeZN()
{
	RotateShapeWithRotation(FIntVector(0, 0, 1), -1);
}

void ABaseShape::RotateShapeWithRotation(FIntVector Axis, int Amount)
{
	if (ensure(GetController()))
	{
		RotateShapeWithRotation(Axis, Amount, Cast<ATetrisPlayerState>(GetController()->PlayerState)->GetRotationZ());
	}
}

void ABaseShape::RotateShapeWithRotation(FIntVector Axis, int Amount, float RotationZ)
{
	float Quadrant = GetRotationQuadrant(RotationZ);
	FVector RotatedAmount = FVector(Axis).RotateAngleAxis(90.0f * Quadrant, FVector(0, 0, 1));
	Server_TryRotateShape(FIntVector(
		FMath::RoundToInt(RotatedAmount.X),
		FMath::RoundToInt(RotatedAmount.Y),
		FMath::RoundToInt(RotatedAmount.Z)
	), Amount);
}

void ABaseShape::Drop()
{
	Server_Drop();
}

bool ABaseShape::Server_Drop_Validate()
{
	return true;
}

void ABaseShape::Server_Drop_Implementation()
{
	bWasDropped = true;
	SetPositionAsShadow();
	ApplyShape();
	ShapeMoved.Broadcast();
}

void ABaseShape::DropOneLevel()
{
	LastTimeDropped = GetWorld()->GetTimeSeconds();
	if (!TryMoveShape(FIntVector(0, 0, -1)))
	{
		ApplyShape();
		ShapeDone.Broadcast(this);
	}
}

void ABaseShape::ApplyShape()
{
	ATetris3DGameModeBase* GameMode = Cast<ATetris3DGameModeBase>(GetWorld()->GetAuthGameMode());
	if (ensure(GameMode))
	{
		GameMode->ApplyShape(this);
		bWasApplied = true;
	}
}

void ABaseShape::ParentShapeMoved()
{
	SetPositionAsShadow();
	ShapeMoved.Broadcast();
}

void ABaseShape::ParentShapeRotated(FVector Axis)
{
	Shape = ParentShape->Shape;
	SetPositionAsShadow();
	ShapeRotated.Broadcast(Axis);
	ShapeMoved.Broadcast();
}

bool ABaseShape::Server_TryMoveShape_Validate(FIntVector Amount)
{
	return Amount.Z == 0 && Amount.X >= -1 && Amount.X <= 1 && Amount.Y >= -1 && Amount.Y <= 1;
}

bool ABaseShape::Server_TryRotateShape_Validate(FIntVector Axis, int Amount)
{
	return ((FMath::Abs(Axis.X) == 1 && Axis.Y == 0 && Axis.Z == 0) ||
			(Axis.X == 0 && FMath::Abs(Axis.Y) == 1 && Axis.Z == 0) ||
			(Axis.X == 0 && Axis.Y == 0 && FMath::Abs(Axis.Z) == 1)) &&
		(Amount == 1 || Amount == -1);
}

void ABaseShape::DoRotateShape(FIntVector Axis, int Amount)
{
	for (int i = 0; i < Shape.Num(); i++)
	{
		FVector CurrentBlock(Shape[i]);
		CurrentBlock = CurrentBlock.RotateAngleAxis(90.0f * (float)Amount, FVector(Axis));
		Shape[i] = FIntVector(
			FMath::RoundToInt(CurrentBlock.X),
			FMath::RoundToInt(CurrentBlock.Y),
			FMath::RoundToInt(CurrentBlock.Z)
		);
	}
}

bool ABaseShape::HasCommonPositionsInMove(TArray<FIntVector> Shape1, TArray<FIntVector> Shape2, FIntVector Shape2Move)
{
	TArray<FIntVector> ClonedShape2;
	for (FIntVector ThePosition : Shape2)
	{
		ClonedShape2.Add(ThePosition + Shape2Move);
	}
	for (const FIntVector& T1 : Shape1)
	{
		if (ClonedShape2.Contains(T1))
		{
			return true;
		}
	}
	return false;
}

void ABaseShape::Server_TryRotateShape_Implementation(FIntVector Axis, int Amount)
{
	DoRotateShape(Axis, Amount);
	bool bDidRotate = false;
	if (!IsValidPosition())
	{
		// Try move shape to accomodate for rotation
		TArray<FIntVector> Directions = { FIntVector(1, 0, 0), FIntVector(0, 1, 0), FIntVector(-1, 0, 0), FIntVector(0, -1, 0) };
		for (int i=0; i<4 && !bDidRotate; i++)
		{
			for (FIntVector Direction : Directions)
			{
				Direction *= i;
				if (HasCommonPositionsInMove(Shape, Shape, Direction) && TryMoveShape(Direction))
				{
					bDidRotate = true;
					break;
				}
			}
		}
	}
	else
	{
		bDidRotate = true;
	}
	if (bDidRotate)
	{
		ShapeRotated.Broadcast(FVector(Axis * Amount));
	} else
	{
		DoRotateShape(Axis, -Amount);
	}
}

void ABaseShape::Server_TryMoveShape_Implementation(FIntVector Amount)
{
	TryMoveShape(Amount);
}

bool ABaseShape::TryMoveShape(FIntVector Amount)
{
	Position += Amount;
	if (!IsValidPosition()) {
		Position -= Amount;
		return false;
	}
	else {
		ShapeMoved.Broadcast();
		return true;
	}
}

bool ABaseShape::IsValidPosition()
{
	bool Okay = true;
	for (FIntVector& BlockPos : Shape)
	{
		Okay = Okay && GameState->IsValidPosition(BlockPos + Position);
	}
	return Okay;
}

// Called every frame
void ABaseShape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsShadow && (ParentShape == nullptr || ParentShape->bWasApplied))
	{
		Destroy();
		return;
	}
	if (!bWasApplied && !bIsShadow)
	{
		DropSpeed = GameState->GetDropSpeed();
		float NextDropTime = 1.0f / DropSpeed;
		float TimeSinceLastDrop = GetWorld()->GetTimeSeconds() - LastTimeDropped;
		if (HasAuthority() && TimeSinceLastDrop > NextDropTime)
		{
			DropOneLevel();
		}
	}
}

// Called to bind functionality to input
void ABaseShape::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("MoveBlockX+", EInputEvent::IE_Pressed, this, &ABaseShape::MoveShapeXP);
	PlayerInputComponent->BindAction("MoveBlockX-", EInputEvent::IE_Pressed, this, &ABaseShape::MoveShapeXN);
	PlayerInputComponent->BindAction("MoveBlockY+", EInputEvent::IE_Pressed, this, &ABaseShape::MoveShapeYP);
	PlayerInputComponent->BindAction("MoveBlockY-", EInputEvent::IE_Pressed, this, &ABaseShape::MoveShapeYN);
	PlayerInputComponent->BindAction("RotateBlockX+", EInputEvent::IE_Pressed, this, &ABaseShape::RotateShapeXP);
	PlayerInputComponent->BindAction("RotateBlockY+", EInputEvent::IE_Pressed, this, &ABaseShape::RotateShapeYP);
	PlayerInputComponent->BindAction("RotateBlockZ+", EInputEvent::IE_Pressed, this, &ABaseShape::RotateShapeZP);
	PlayerInputComponent->BindAction("RotateBlockX-", EInputEvent::IE_Pressed, this, &ABaseShape::RotateShapeXN);
	PlayerInputComponent->BindAction("RotateBlockY-", EInputEvent::IE_Pressed, this, &ABaseShape::RotateShapeYN);
	PlayerInputComponent->BindAction("RotateBlockZ-", EInputEvent::IE_Pressed, this, &ABaseShape::RotateShapeZN);
	PlayerInputComponent->BindAction("Drop", EInputEvent::IE_Pressed, this, &ABaseShape::Drop);
}

FVector ABaseShape::GetWorldLocation()
{
	return GetBlockWorldLocation(ShapeStart.Get(), BlockSize, Position);
}

FIntVector ABaseShape::GetPosition()
{
	return Position;
}

