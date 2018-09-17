// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisCamera.h"
#include "TetrisPlayerState.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/Controller.h>

void ATetrisCamera::UpdateCamera()
{
	if (ensureMsgf(PlayerState, TEXT("Player state not set"))) {
		float RotationZD = FMath::DegreesToRadians(PlayerState->GetRotationZ());
		float ElevationD = FMath::DegreesToRadians(Elevation);
		FVector Location(
			FMath::Cos(RotationZD) * Distance * FMath::Cos(ElevationD),
			FMath::Sin(RotationZD) * Distance * FMath::Cos(ElevationD),
			FMath::Sin(ElevationD) * Distance
		);
		FRotator Rotation = FRotationMatrix::MakeFromX(-Location).Rotator();

		GetCameraComponent()->SetRelativeLocationAndRotation(Location, Rotation);
	}
}

void ATetrisCamera::SetController(AController* ControllerToSet)
{
	PlayerState = Cast<ATetrisPlayerState>(ControllerToSet->PlayerState);
	UpdateCamera();
}

void ATetrisCamera::AddDistance(float amount) {
	if (amount != 0) {
		Distance = FMath::Clamp(Distance + amount, MinDistance, MaxDistance);
		UpdateCamera();
	}
}

void ATetrisCamera::AddElevation(float amount) {
	if (amount != 0) {
		Elevation = FMath::Clamp(Elevation + amount, MinElevation, MaxElevation);
		UpdateCamera();
	}
}

void ATetrisCamera::AddRotation(float amount) {
	if (amount != 0 && ensureMsgf(PlayerState, TEXT("Player state not set"))) {
		PlayerState->AddRotationZ(amount);
		UpdateCamera();
	}
}