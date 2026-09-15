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
		OtherPlayer = *ActorItr;
		if (OtherPlayer && OtherPlayer != PlayerCharacter)
		{
			OtherPlayer = Cast<APlayerCharacter>(OtherPlayer);
			break;
		}
	}
	if (OtherPlayer == nullptr)
	{
		return;
	}
	
	OtherPlayer->SetSlowAmount(SlowAmount);
	
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, OtherPlayer]()
	{
		OtherPlayer->SetSlowAmount(1) ;
		
		this->DestroyComponent();
	});
	
	GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
}
			
		
	

