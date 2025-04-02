// Fill out your copyright notice in the Description page of Project Settings.


#include "AddScore_EffectComponent.h"
#include "PlayerCharacter.h"
//#include "ABP_GameMode.h"
void UAddScore_EffectComponent::ApplyEffect(AActor* Actor)
{
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			int CurrentScore = PlayerCharacter->GetScore();
			PlayerCharacter->SetScore(CurrentScore + ScoreToAdd);
			//ABP_GameMode* GameMode = GetWorld()->GetAuthGameMode<ABP_GameMode>();
			//if (GameMode != nullptr)
			//{
			//GameMode->CheckWinCondition();
			//}
		}
	}
		
	Super::ApplyEffect(Actor);
}
