// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomTeleport_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"
#include "MovementNode.h"

void URandomTeleport_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);

	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				APlayerCharacter* OtherPlayer = *ActorItr;
				if (OtherPlayer != nullptr && OtherPlayer != PlayerCharacter)
				{
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
					//UE_LOG(LogTemp, Warning, TEXT("Player 1: %s Player 2 :"), *PlayerCharacter->GetName(),  );
					
				}
			}
		}
	}
	this->DestroyComponent();
}
