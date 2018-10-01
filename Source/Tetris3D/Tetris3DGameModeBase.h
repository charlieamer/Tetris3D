// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Tetris3DGameModeBase.generated.h"

class ATetrisGameState;

UCLASS()
class TETRIS3D_API ATetris3DGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	void SpawnNewShape(class AController* ShapeController);
	void SpawnNewShape(TArray<class AController*> ShapeControllers);

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class ABaseShape>> PossibleShapes;
public:
	void InitGameState() override;
	ATetris3DGameModeBase();
	ATetrisGameState* GetTetrisState();

	void ApplyShape(class ABaseShape* Shape);
};
