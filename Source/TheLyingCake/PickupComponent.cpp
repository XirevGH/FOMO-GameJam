// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupComponent.h"
#include "PlayerCharacter.h"
#include "EffectComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "CakeChaseGameMode.h"
#include "Particles/ParticleSystemComponent.h"

void UPickupComponent::BeginPlay()
{
    Super::BeginPlay();

    // Register our Overlap Event
    UE_LOG(LogTemp, Warning, TEXT("Pickup component initialized for %s"), *GetOwner()->GetName());
    OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::OnPickup);
    
    // Ensure we can only be collected once
    bHasBeenCollected = false;
}

void UPickupComponent::ApplyEffectToActor(AActor* ActorToApply)
{
    if (!ActorToApply || !EffectClass) return;
    
    UEffectComponent* Effect = NewObject<UEffectComponent>(ActorToApply, EffectClass);
    if (Effect)
    {
        Effect->RegisterComponent();
        Effect->ApplyEffect(ActorToApply);
    }
}

void UPickupComponent::OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Prevent collection if already collected or invalid actor
    if (bHasBeenCollected || !OtherActor || !OtherActor->IsValidLowLevel())
    {
        return;
    }
    
    // Check if the actor is a player character
    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OtherActor);
    if (!PlayerChar)
    {
        return;
    }
    
    // Mark as collected immediately to prevent multiple triggers
    bHasBeenCollected = true;
    
    // Log collection
    UE_LOG(LogTemp, Warning, TEXT("Player, %s, picked up the item %s"), 
        *OtherActor->GetName(), *GetOwner()->GetName());
        
    // Apply the effect
    ApplyEffectToActor(OtherActor);
    
    // Notify the game mode - using a local variable to avoid issues if object gets destroyed
    ACakeChaseGameMode* CakeChaseGameMode = Cast<ACakeChaseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (CakeChaseGameMode)
    {
        // Notify the Game Mode that this cake was collected
        CakeChaseGameMode->NotifyCakeCollected();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PickupComponent could not find CakeChaseGameMode!"));
    }
    
    // Spawn particles if defined
    if (PicUpParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(), 
            PicUpParticle, 
            GetOwner()->GetActorLocation(), 
            GetOwner()->GetActorRotation()
        );
    }
    
    // Store owner in a local variable because we'll destroy it
    AActor* Owner = GetOwner();
    if (Owner && Owner->IsValidLowLevel())
    {
        // Use a delayed destroy to prevent any ongoing calculations from causing issues
        Owner->SetLifeSpan(0.01f); // Small delay to allow current frame to complete
    }
}