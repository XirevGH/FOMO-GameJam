// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowEffectComponent.h"

void USlowEffectComponent::ApplyEffect(AActor* Actor)
{
	
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Display, TEXT("ApplyEffect: Slow"));	
	
}
