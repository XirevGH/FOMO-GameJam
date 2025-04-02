// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PickupComponent.generated.h"

class UEffectComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THELYINGCAKE_API UPickupComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	// Flag to prevent multiple collections
	bool bHasBeenCollected;
    
	// The effect to apply when picked up
	UPROPERTY(EditAnywhere, Category = "Pickup")
	TSubclassOf<UEffectComponent> EffectClass;
    
	// Optional particle effect when picked up
	UPROPERTY(EditAnywhere, Category = "Pickup")
	class UParticleSystem* PicUpParticle;
    
	// Function to apply the effect to an actor
	void ApplyEffectToActor(AActor* ActorToApply);
    
	// Overlap event handler
	UFUNCTION()
	void OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};