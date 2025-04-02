// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowEffectComponent.h"
#include "PlayerCharacter.h"


void USlowEffectComponent::ApplyEffect(AActor* Actor)
{
	
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Display, TEXT("ApplyEffect: Slow"));
	APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);
	EffectDuration = 5;
	//Slow the player
	//Player->SetSpeed(1);
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this]()
	{
		//Player->SetSpeed(Player->StartSpeed);
		// player effect or sound ?
		
		this->DestroyComponent();
	});
	
	GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
	
}
