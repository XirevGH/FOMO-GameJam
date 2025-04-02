// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CakeChaseGameMode.generated.h"

class MovementNode;

/**
 * 
 */
UCLASS()
class THELYINGCAKE_API ACakeChaseGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
private:
	void FindAllMovementNodes() const;
	TArray<AActor*> MovementNodes;

	void SpawnCakes();
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 MaximumCakes;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 MinimumCakes;

	int32 CurrentAmountCakes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess), Category="Cake Spawns")
	TSubclassOf<AActor> MovementNodeBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess), Category="Cake Spawns")
	TSubclassOf<AActor> CakeBP;
};
