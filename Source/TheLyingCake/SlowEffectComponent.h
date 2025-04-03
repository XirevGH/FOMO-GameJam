// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectComponent.h"
#include "SlowEffectComponent.generated.h"

/**
 * 
 */
UCLASS()
class THELYINGCAKE_API USlowEffectComponent : public UEffectComponent
{
	GENERATED_BODY()
public:
	virtual void ApplyEffect(AActor* Actor) override;

private:
	float SlowAmount = 20;
};

