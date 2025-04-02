// Fill out your copyright notice in the Description page of Project Settings.


#include "CakeChaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MovementNode.h"


void ACakeChaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	FindAllMovementNodes();
}

void ACakeChaseGameMode::FindAllMovementNodes() const
{

	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), MovementNodeBP, MovementNodes);
}

void ACakeChaseGameMode::SpawnCakes()
{
	int32 RandomNodeIndex = FMath::RandRange(0, MovementNodes.Num()-1);
}
