// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisGameState.h"

#define VALID_POSITION_CONSTRAINT (X >= 0 && X < SizeX && Y >= 0 && Y < SizeY && Z >= 0 && Z < SizeZ && ensure(Fields))

int ATetrisGameState::GetIndex(int X, int Y, int Z)
{
	if (ensure(VALID_POSITION_CONSTRAINT)) {
		return X + SizeX * Y + SizeX * SizeY * Z;
	}
	else {
		return 0;
	}
}

void ATetrisGameState::Initialize(int SizeX, int SizeY, int SizeZ, float BlockSize)
{
	this->SizeX = SizeX;
	this->SizeY = SizeY;
	this->SizeZ = SizeZ;
	this->BlockSize = BlockSize;
	bool* Mem = new bool[SizeX * SizeY * SizeZ];
	for (int i = 0; i < SizeX * SizeY * SizeZ; i++)
	{
		Mem[i] = false;
	}
	Fields = MakeShareable(Mem);
}

bool ATetrisGameState::IsValidPosition(int X, int Y, int Z)
{
	return VALID_POSITION_CONSTRAINT && !((&*Fields)[GetIndex(X, Y, Z)]);
}

bool ATetrisGameState::IsValidPosition(const FIntVector& Position)
{
	return IsValidPosition(Position.X, Position.Y, Position.Z);
}

float ATetrisGameState::GetBlockSize()
{
	return BlockSize;
}

int ATetrisGameState::GetSizeX()
{
	return SizeX;
}

int ATetrisGameState::GetSizeY()
{
	return SizeY;
}

int ATetrisGameState::GetSizeZ()
{
	return SizeZ;
}

