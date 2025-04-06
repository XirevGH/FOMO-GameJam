// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchScore_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

void USwitchScore_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);
	if (Actor == nullptr)
	{
		return;
	}
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
	if (PlayerCharacter != nullptr)
	{
		return;
	}
	APlayerCharacter* OtherPlayer = nullptr;
	for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		OtherPlayer = *ActorItr;
		if (OtherPlayer != nullptr && OtherPlayer != PlayerCharacter)
		{
			OtherPlayer = Cast<APlayerCharacter>(OtherPlayer);
			break;
		}
	}
	
	int TempScore = PlayerCharacter->GetScore();
	int OtherPlayerScore = OtherPlayer->GetScore();
	PlayerCharacter->SetScore( OtherPlayerScore);
	OtherPlayer->SetScore( TempScore);
				
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { this->DestroyComponent(); });
}
