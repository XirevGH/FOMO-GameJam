// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectComponent.h"
#include "AddScore_EffectComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THELYINGCAKE_API UAddScore_EffectComponent : public UEffectComponent
{
	GENERATED_BODY()
	public:
	void ApplyEffect(AActor* Actor) override;
private:
	UPROPERTY(VisibleAnywhere)
	int ScoreToAdd{1};
	
};
