// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

int GetRotationQuadrant(float RotationZ);
FVector GetBlockWorldLocation(class UWorld* World, FIntVector BlockLocation);
FVector GetBlockWorldLocation(class APlayerStart* PlayerStart, float BlockSize, FIntVector BlockLocation);
void SetupBlockSceneComponent(class UStaticMeshComponent* SpawnedMesh, float BlockSize, FIntVector BlockLocation);
void SetupBlockSceneComponent(class UStaticMeshComponent* SpawnedMesh, class UWorld* World, FIntVector BlockLocation);