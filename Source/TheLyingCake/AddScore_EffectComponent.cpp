// Fill out your copyright notice in the Description page of Project Settings.


#include "AddScore_EffectComponent.h"
#include "PlayerCharacter.h"

void UAddScore_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			int CurrentScore = PlayerCharacter->GetScore();
			PlayerCharacter->SetScore(CurrentScore + ScoreToAdd);
			
			this->DestroyComponent();
		}
	}
		
	
}
