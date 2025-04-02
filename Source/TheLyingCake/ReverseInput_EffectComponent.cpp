// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseInput_EffectComponent.h"
#include "PlayerCharacter.h"

void UReverseInput_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);

	EffectDuration = 0;
	
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Display, TEXT("ApplyEffect: Slow"));
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			//reverse the input
			float OriginalSpeed = PlayerCharacter->GetMovementSpeed();
			
			UE_LOG(LogTemp, Warning, TEXT(""));
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, PlayerCharacter, OriginalSpeed]()
			{

				//reset input
				PlayerCharacter->SetMovementSpeed(OriginalSpeed);
		
				this->DestroyComponent();
			});
			GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
		}
	}
	
}
