// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowEffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

void USlowEffectComponent::ApplyEffect(AActor* Actor)
{
	EffectDuration = 10;
	
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Warning, TEXT("ApplyEffect: Slow"));
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			//Slow the player
			for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				APlayerCharacter* OtherPlayer = *ActorItr;
				if (OtherPlayer != nullptr && OtherPlayer != PlayerCharacter)
				{
					OtherPlayer->SetSlowAmount(SlowAmount);
					FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, OtherPlayer]()
					{
						//Player->SetSpeed(Player->StartSpeed);
						// player effect or sound ?
						OtherPlayer->SetSlowAmount(1) ;
				
						this->DestroyComponent();
					});
					GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
				}
			}
		}
	}
	
	
	
	
}
