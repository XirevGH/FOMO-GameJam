// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomTeleport_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"
#include "MovementNode.h"

void URandomTeleport_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);

	if (Actor == nullptr)
	{
		return;
	}
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
	if (PlayerCharacter == nullptr)
	{
		return;
	}
	
	APlayerCharacter* OtherPlayer = nullptr;
	for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		OtherPlayer = *ActorItr;
		// Skip if null or if it's the triggering player
		if (OtherPlayer && OtherPlayer != PlayerCharacter)
		{
			OtherPlayer = Cast<APlayerCharacter>(OtherPlayer);
			break; // Found the opponent, no need to continue loop
		}
	}
	
	if (OtherPlayer == nullptr)
	{
		return;
	}
	OtherPlayer->SetIsMoving(false);
	
	//find node
	TArray<AMovementNode*> NodeList;
	for (TActorIterator<AMovementNode> NodeItr(GetWorld()); NodeItr; ++NodeItr)
	{
		AMovementNode* Node = *NodeItr;
		if (Node)
		{
			NodeList.Add(Node);
		}
	}

	if (NodeList.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, NodeList.Num() - 1);
		AMovementNode* RandomNode = NodeList[RandomIndex];

		OtherPlayer->SetCurrentNode(RandomNode);
		
		OtherPlayer->SetActorLocation(Cast<AActor>(RandomNode)->GetActorLocation());
	}
	
	this->DestroyComponent();
}
