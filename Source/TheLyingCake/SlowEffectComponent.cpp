// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowEffectComponent.h"
#include "PlayerCharacter.h"


void USlowEffectComponent::ApplyEffect(AActor* Actor)
{
	EffectDuration = 5;
	
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Warning, TEXT("ApplyEffect: Slow"));
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			//Slow the player
			float OriginalSpeed = PlayerCharacter->GetMovementSpeed();
			PlayerCharacter->SetMovementSpeed(10);
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, PlayerCharacter, OriginalSpeed]()
			{
				//Player->SetSpeed(Player->StartSpeed);
				// player effect or sound ?
				PlayerCharacter->SetMovementSpeed(OriginalSpeed);
		
				this->DestroyComponent();
			});
			GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
		}
	}
	
	
	
	
}
