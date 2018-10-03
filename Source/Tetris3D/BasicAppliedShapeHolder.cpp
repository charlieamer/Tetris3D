// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicAppliedShapeHolder.h"
#include <Engine/World.h>
#include <Rules/TetrisGameState.h>
#include <BaseShape.h>
#include <Components/StaticMeshComponent.h>
#include <TetrisStatics.h>
#include <Materials/MaterialInstance.h>

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
		for (FIntVector BlockLocation : Shape->GetShape())
		{
			UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this);
			Mesh->RegisterComponent();
			Mesh->SetStaticMesh(MeshToSpawn);
			Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			SetupBlockSceneComponent(Mesh, GetWorld(), BlockLocation + Shape->GetPosition());
			// Mesh->SetMaterial(0, Shape->MaterialToUse);
		}
	}
	Shape->Destroy();
}