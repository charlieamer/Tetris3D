// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseShape.h"
#include "Rules/TetrisGameState.h"
#include <Components/StaticMeshComponent.h>
#include <Rules/TetrisGameState.h>
#include <GameFramework/PlayerStart.h>
#include <Tetris3DGameModeBase.h>

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
	SetPositionAsShadow();
}

// Called when the game starts or when spawned
void ABaseShape::BeginPlay()
{
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
	}
	Super::BeginPlay();
}

void ABaseShape::InitializeShapeMeshes()
{
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
	if (!ParentShape->IsValidPosition())
	{
		Destroy();
		return;
	}
	for (Position = ParentShape->GetPosition(); Position.Z > 0; Position.Z--)
	{
		if (!IsValidPosition())
		{
			Position.Z++;
			break;
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
		ApplyShape();
	}
}

void ABaseShape::ApplyShape()
{
	ATetris3DGameModeBase* GameMode = Cast<ATetris3DGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->ApplyShape(this);
		bWasApplied = true;
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
	if (bIsShadow && !ParentShape->IsValidLowLevel())
	{
		Destroy();
		return;
	}
	if (!bWasApplied)
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
}

FVector ABaseShape::GetWorldLocation()
{
	FVector LocalLocation = FVector(Position) * BlockSize;
	return ShapeStart->GetActorTransform().TransformPosition(LocalLocation);
}

FIntVector ABaseShape::GetPosition()
{
	return Position;
}

