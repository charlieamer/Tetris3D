// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisPlayerState.h"

void ATetrisPlayerState::AddRotationZ(float amount)
{
	RotationZ += amount;
}

float ATetrisPlayerState::GetRotationZ()
{
	return RotationZ;
}
