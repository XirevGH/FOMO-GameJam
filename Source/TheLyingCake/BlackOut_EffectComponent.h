// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectComponent.h"
#include "BlackOut_EffectComponent.generated.h"

/**
 * 
 */
UCLASS()
class THELYINGCAKE_API UBlackOut_EffectComponent : public UEffectComponent
{
	GENERATED_BODY()
public:
	void ApplyEffect(AActor* Actor) override;
protected:
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
};
