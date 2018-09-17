// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseShape.h"
#include "Rules/TetrisGameState.h"
#include <Components/StaticMeshComponent.h>
#include <Rules/TetrisGameState.h>
#include <GameFramework/PlayerStart.h>

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
	BlockSize = (Cast<ATetrisGameState>(GetWorld()->GetGameState()))->GetBlockSize();
	DropSpeed = (Cast<ATetrisGameState>(GetWorld()->GetGameState()))->GetDropSpeed();
	LastTimeDropped = GetWorld()->GetTimeSeconds();

	for (FIntVector ShapeBlock : Shape)
	{
		UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this);
		Mesh->RegisterComponent();
		Mesh->SetStaticMesh(MeshToSpawn);
		Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		
		FBox BoundingBox = MeshToSpawn->GetBoundingBox();
		FVector Bounds = BoundingBox.GetSize();
		FVector Scale = Bounds * (1.0f / BlockSize);
		Mesh->SetRelativeScale3D(Scale);
		Mesh->SetRelativeLocation(FVector(ShapeBlock) * BlockSize);
	}

	SetStartingPosition();
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
			(Cast<ATetrisGameState>(GetWorld()->GetGameState()))->GetSizeX() / 2,
			(Cast<ATetrisGameState>(GetWorld()->GetGameState()))->GetSizeY() / 2,
			(Cast<ATetrisGameState>(GetWorld()->GetGameState()))->GetSizeZ() - HighestZ - 1
		);
		if (!IsValidPosition())
		{
			Destroy();
			UE_LOG(LogTemp, Error, TEXT("GAME OVER"));
		}
	}
}

void ABaseShape::MoveShapeXP()
{
	Server_TryMoveShape(FIntVector(1, 0, 0));
}

void ABaseShape::MoveShapeXN()
{
	Server_TryMoveShape(FIntVector(-1, 0, 0));
}

void ABaseShape::MoveShapeYP()
{
	Server_TryMoveShape(FIntVector(0, -1, 0));
}

void ABaseShape::MoveShapeYN()
{
	Server_TryMoveShape(FIntVector(0, 1, 0));
}

void ABaseShape::DropOneLevel()
{
	LastTimeDropped = GetWorld()->GetTimeSeconds();
	if (!TryMoveShape(FIntVector(0, 0, -1)))
	{
		Destroy();
	}
}
bool ABaseShape::Server_TryMoveShape_Validate(FIntVector Amount)
{
	return Amount.Z == 0 && Amount.X >= -1 && Amount.X <= 1 && Amount.Y >= -1 && Amount.Y <= 1;
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
		return true;
	}
}

bool ABaseShape::IsValidPosition()
{
	bool Okay = true;
	ATetrisGameState* GameState = Cast<ATetrisGameState>(GetWorld()->GetGameState());
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
	if (HasAuthority() && GetWorld()->GetTimeSeconds() - LastTimeDropped > 1.0f / DropSpeed)
	{
		DropOneLevel();
	}
	if (!FPlatformProperties::IsServerOnly())
	{
		SetActorLocation(GetWorldLocation());
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
}

FVector ABaseShape::GetWorldLocation()
{
	FVector LocalLocation = FVector(Position) * BlockSize;
	return ShapeStart->GetActorTransform().TransformPosition(LocalLocation);;
}

