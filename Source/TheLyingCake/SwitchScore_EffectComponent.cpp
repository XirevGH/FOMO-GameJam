// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchScore_EffectComponent.h"
#include "PlayerCharacter.h"

void USwitchScore_EffectComponent::ApplyEffect(AActor* Actor)
{
	
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			int tempSocre = PlayerCharacter->GetScore();
			//GetWorld()->GetAuthGameMode()->getPl
			//int otherPlayerScore = PlayerCharacter_2->GetScore();
			//PlayerCharacter->SetScore( otherPlayerScore);
			//PlayerCharacter_2->SetScore( tempSocre);
			UE_LOG(LogTemp, Warning, TEXT("Player 1: %s Player 2 :"), *PlayerCharacter->GetName(),  );
			
			
		}
	}
	Super::ApplyEffect(Actor);
}
