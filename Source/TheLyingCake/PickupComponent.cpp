// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupComponent.h"
#include "PlayerCharacter.h"
#include "EffectComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "CakeChaseGameMode.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

void UPickupComponent::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("Pickup component initialized for %s"), *GetOwner()->GetName());
    OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::OnPickup);
    
    bHasBeenCollected = false;
}

void UPickupComponent::ApplyEffectToActor(AActor* ActorToApply)
{
    if (!ActorToApply || !EffectClass) return;
    
    UEffectComponent* Effect = NewObject<UEffectComponent>(ActorToApply, EffectClass);
    if (Effect)
    {
        Effect->RegisterComponent();
        Effect->SetEffectDuration(EffectDuration);
        Effect->ApplyEffect(ActorToApply);
    }
}

void UPickupComponent::OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bHasBeenCollected || !OtherActor || !OtherActor->IsValidLowLevel())
    {
        return;
    }
    
    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OtherActor);
    if (!PlayerChar)
    {
        return;
    }
    
    bHasBeenCollected = true;
    
    UE_LOG(LogTemp, Warning, TEXT("Player, %s, picked up the item %s"), 
        *OtherActor->GetName(), *GetOwner()->GetName());
    
    ApplyEffectToActor(OtherActor);
    
    ACakeChaseGameMode* CakeChaseGameMode = Cast<ACakeChaseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (CakeChaseGameMode)
    {
        CakeChaseGameMode->NotifyCakeCollected();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PickupComponent could not find CakeChaseGameMode!"));
    }
    
    if (PicUpParticle)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), 
            PicUpParticle, 
            GetOwner()->GetActorLocation(), 
            GetOwner()->GetActorRotation()
        );
    }

    if (SoundEffect)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), SoundEffect);
    }
    
    AActor* Owner = GetOwner();
    if (Owner && Owner->IsValidLowLevel())
    {
        Owner->SetLifeSpan(0.01f);
    }
}