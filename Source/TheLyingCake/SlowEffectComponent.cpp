// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowEffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

void USlowEffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Warning, TEXT("ApplyEffect: Slow"));
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
		APlayerCharacter* CurrentPlayer = *ActorItr;
		// Skip if null or if it's the triggering player
		if (CurrentPlayer && CurrentPlayer != PlayerCharacter)
		{
			OtherPlayer = Cast<APlayerCharacter>(CurrentPlayer);
			break; // Found the opponent, no need to continue loop
		}
	}
	if (OtherPlayer == nullptr)
	{
		return;
	}
	OtherPlayer->SetSlowAmount(SlowAmount);
	//Slow the player
			
					FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, OtherPlayer]()
					{
						//Player->SetSpeed(Player->StartSpeed);
						// player effect or sound ?
						OtherPlayer->SetSlowAmount(1) ;
				
						this->DestroyComponent();
					});
					GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
}
			
		
	

