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
	void FindAllMovementNodes();
	TArray<MovementNode*> MovementNodes;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess), Category="Movement Node")
	TSubclassOf<AActor> MovementNodeBP;
};
