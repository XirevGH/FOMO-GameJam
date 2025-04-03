// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchScore_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

void USwitchScore_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				APlayerCharacter* OtherPlayer = Cast<APlayerCharacter>(*ActorItr);
				if (OtherPlayer != PlayerCharacter)
				{
					int TempSocre = PlayerCharacter->GetScore();
					int OtherPlayerScore = OtherPlayer->GetScore();
					PlayerCharacter->SetScore( OtherPlayerScore);
					OtherPlayer->SetScore( TempSocre);
					//UE_LOG(LogTemp, Warning, TEXT("Player 1: %s Player 2 :"), *PlayerCharacter->GetName(),  );
					this->DestroyComponent();
				}
			}
		}
	}
	
}
