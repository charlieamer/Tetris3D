// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicAppliedShapeHolder.h"
#include <Engine/World.h>
#include <Rules/TetrisGameState.h>
#include <BaseShape.h>
#include <Components/StaticMeshComponent.h>
#include <TetrisStatics.h>
#include <Materials/MaterialInstance.h>
#include <GameFramework/PlayerStart.h>

ABasicAppliedShapeHolder::ABasicAppliedShapeHolder()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

// Called when the game starts or when spawned
void ABasicAppliedShapeHolder::BeginPlay()
{
	Super::BeginPlay();
	ATetrisGameState* GameState = Cast<ATetrisGameState>(GetWorld()->GetGameState());
	if (ensure(GameState))
	{
		GameState->ShapeApplied.AddDynamic(this, &ABasicAppliedShapeHolder::ShapeApplied);
		GameState->LevelFilled.AddDynamic(this, &ABasicAppliedShapeHolder::OnLevelsDestroyed);
	}	
}

void ABasicAppliedShapeHolder::ShapeApplied(ABaseShape* Shape)
{
	Shape->ShapeDone.AddDynamic(this, &ABasicAppliedShapeHolder::ShapeDone);
}

void ABasicAppliedShapeHolder::ShapeDone(ABaseShape* Shape)
{
	if (ensure(MeshToSpawn))
	{
		if (LevelDestroyQueue.Num() == 0)
		{
			for (FIntVector BlockLocation : Shape->GetShape())
			{
				AddCube(BlockLocation + Shape->GetPosition());
			}
		}
		else
		{
			TArray<UStaticMeshComponent*> CurrentMeshes;
			GetComponents<UStaticMeshComponent>(CurrentMeshes);
			for (UStaticMeshComponent* MeshComponent : CurrentMeshes)
			{
				MeshComponent->DestroyComponent();
			}
			ATetrisGameState* GameState = Cast<ATetrisGameState>(GetWorld()->GetGameState());
			if (ensure(GameState))
			{
				for (int z=0; z<GameState->GetSizeZ(); z++)
				{
					bool ShouldDestroy = LevelDestroyQueue.Contains(z);
					APlayerStart* PlayerStart = *TObjectIterator<APlayerStart>();
					for (int x=0; x<GameState->GetSizeX(); x++)
					{
						for (int y=0; y<GameState->GetSizeY(); y++)
						{
							FIntVector Location(x, y, z);
							if (GameState->IsUsed(x, y, z))
							{
								AddCube(Location);
							}
							if (ShouldDestroy && ensure(ActorWhenBlockDestroyed) &&
								(x == 0 || y == 0 || x == GameState->GetSizeX() - 1 || y == GameState->GetSizeY() - 1))
							{
								FVector EffectLocation = GetBlockWorldLocation(PlayerStart, GameState->GetBlockSize(), Location);
								AActor* SpawnedEffect = GetWorld()->SpawnActor<AActor>(
									ActorWhenBlockDestroyed
								);
								SpawnedEffect->SetActorLocation(EffectLocation);
							}
						}
					}
				}
				LevelDestroyQueue.Reset();
			}
		}
	}
	Shape->Destroy();
}

void ABasicAppliedShapeHolder::OnLevelsDestroyed(TArray<int> Levels)
{
	for (int Level : Levels)
	{
		LevelDestroyQueue.AddUnique(Level);
	}
}

void ABasicAppliedShapeHolder::AddCube(FIntVector Location)
{
	UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this);
	Mesh->RegisterComponent();
	Mesh->SetStaticMesh(MeshToSpawn);
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SetupBlockSceneComponent(Mesh, GetWorld(), Location);
	// Mesh->SetMaterial(0, Shape->MaterialToUse);
}