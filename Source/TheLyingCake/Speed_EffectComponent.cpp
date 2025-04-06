// Fill out your copyright notice in the Description page of Project Settings.


#include "Speed_EffectComponent.h"

#include "PlayerCharacter.h"


void USpeed_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Warning, TEXT("ApplyEffect: Slow"));
	if (Actor == nullptr)
	{
		return;
	}
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
	if (PlayerCharacter != nullptr)
	{
		return;
	}
			//Slow the player
	PlayerCharacter->SetSpeedAmount(SpeedAmount) ;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, PlayerCharacter]()
	{
		
		PlayerCharacter->SetSpeedAmount(1) ;
				
		this->DestroyComponent();
	});
	GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
}
	
	
	
	
	
