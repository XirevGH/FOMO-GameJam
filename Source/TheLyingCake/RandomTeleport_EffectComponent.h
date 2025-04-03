// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectComponent.h"
#include "RandomTeleport_EffectComponent.generated.h"

/**
 * 
 */
UCLASS()
class THELYINGCAKE_API URandomTeleport_EffectComponent : public UEffectComponent
{
	GENERATED_BODY()
	public:
	virtual void ApplyEffect(AActor* Actor) override;
};
