// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectComponent.h"
#include "SwitchScore_EffectComponent.generated.h"

/**
 * 
 */
UCLASS()
class THELYINGCAKE_API USwitchScore_EffectComponent : public UEffectComponent
{
	GENERATED_BODY()

public:
	void ApplyEffect(AActor* Actor) override;
};
