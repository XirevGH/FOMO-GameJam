// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PickupComponent.generated.h"

class UEffectComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THELYINGCAKE_API UPickupComponent : public USphereComponent
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	void ApplyEffectToActor(AActor* ActorToApply);
	
private:
	UFUNCTION()
	void OnPickup(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess))
	TSubclassOf<UEffectComponent> EffectClass;

	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* PicUpParticle;
	
};
