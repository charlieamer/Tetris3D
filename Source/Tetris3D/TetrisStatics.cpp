// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisStatics.h"

int GetRotationQuadrant(float RotationZ)
{
	float Modded = FMath::Fmod(RotationZ, 360.0f);
	if (RotationZ < 0)
	{
		Modded = 360.0f - Modded;
	}
	if (Modded < 45.0f || Modded >= 315.0f)
	{
		return 3;
	}
	if (Modded >= 45.0f && Modded < 135.0f)
	{
		return 0;
	}
	if (Modded >= 135.0f && Modded < 225.0f)
	{
		return 1;
	}
	return 2;
}
