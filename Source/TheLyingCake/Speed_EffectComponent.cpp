// Fill out your copyright notice in the Description page of Project Settings.


#include "Speed_EffectComponent.h"

#include "PlayerCharacter.h"


void USpeed_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Warning, TEXT("ApplyEffect: Slow"));
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			//Slow the player
			PlayerCharacter->SetSpeedAmount(SpeedAmount) ;
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, PlayerCharacter]()
			{
				//Player->SetSpeed(Player->StartSpeed);
				// player effect or sound ?
				PlayerCharacter->SetSpeedAmount(1) ;
				
				this->DestroyComponent();
			});
			GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
		}
	}
	
	
	
	
}