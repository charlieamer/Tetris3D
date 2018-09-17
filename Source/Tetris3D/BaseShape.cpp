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
}

void ABaseShape::MoveShapeXP()
{
	TryMoveShape(FIntVector(1, 0, 0));
}

void ABaseShape::MoveShapeXN()
{
	TryMoveShape(FIntVector(-1, 0, 0));
}

void ABaseShape::MoveShapeYP()
{
	TryMoveShape(FIntVector(0, -1, 0));
}

void ABaseShape::MoveShapeYN()
{
	TryMoveShape(FIntVector(0, 1, 0));
}

void ABaseShape::TryMoveShape(FIntVector Amount)
{
	Position += Amount;
	if (!IsValidPosition()) {
		Position -= Amount;
	}
	else {
		SetActorLocation(UpdateWorldLocation());
	}
}

FVector ABaseShape::UpdateWorldLocation()
{
	FVector LocalLocation = FVector(Position) * BlockSize;
	WorldLocation = ShapeStart->GetActorTransform().TransformPosition(LocalLocation);
	return WorldLocation;
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
	return WorldLocation;
}

