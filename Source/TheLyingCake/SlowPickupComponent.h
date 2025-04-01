// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupComponent.h"
#include "SlowPickupComponent.generated.h"

/**
 * 
 */
UCLASS()
class THELYINGCAKE_API USlowPickupComponent : public UPickupComponent
{
	GENERATED_BODY()

	public:
	void ApplyEffectToActor(AActor* ActorToApply);
};
