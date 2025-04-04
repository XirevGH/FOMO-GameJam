// Copyright YourCompany. All Rights Reserved.

#include "PowerUp.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "CakeChaseGameMode.h"        // Include your GameMode header to notify it
#include "Kismet/GameplayStatics.h" // For GetAuthGameMode
#include "PickupComponent.h"        // !!! IMPORTANT: Replace with the correct path/filename for your PickupComponent header !!!

// --- Constructor ---
APowerUp::APowerUp()
{
    // Optimize: Disable ticking if the power-up doesn't need to update every frame.
    PrimaryActorTick.bCanEverTick = false;

    // 1. Create the Collision Component (USphereComponent)
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    if (CollisionComp)
    {
        RootComponent = CollisionComp;
        CollisionComp->InitSphereRadius(60.0f);
        CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
        CollisionComp->SetGenerateOverlapEvents(true);
        CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APowerUp::OnOverlapBegin);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("APowerUp: Failed to create CollisionComp!"));
    }


    // 2. Create the Static Mesh Component
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    if (MeshComp && RootComponent) // Check if MeshComp and its parent (RootComponent) are valid
    {
        // Attach the Mesh Component to the Root Component (CollisionComp).
        MeshComp->SetupAttachment(RootComponent);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Visual only
    }
     else
    {
         if (!MeshComp) UE_LOG(LogTemp, Error, TEXT("APowerUp: Failed to create MeshComp!"));
         // If RootComponent is null, CollisionComp creation failed (already logged)
    }

    // 3. Create Your Custom Pickup Component
    PickupComp = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComp"));
    if (PickupComp && MeshComp) // Check if PickupComp and its parent (MeshComp) are valid
    {
        // Attach the Pickup Component to the Mesh Component.
        PickupComp->SetupAttachment(MeshComp);
        // Add any specific initialization for your PickupComp here if needed.
        // e.g., PickupComp->SomeCustomVariable = DefaultValue;
    }
    else
    {
         if (!PickupComp) UE_LOG(LogTemp, Error, TEXT("APowerUp: Failed to create PickupComp!"));
         if (!MeshComp) UE_LOG(LogTemp, Error, TEXT("APowerUp: Cannot attach PickupComp because MeshComp is invalid!"));
    }

    // Current Hierarchy:
    // - CollisionComp (Root)
    //   - MeshComp
    //     - PickupComp
}

// --- Overlap Handling (Bound to CollisionComp) ---
void APowerUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // This logic remains bound to the CollisionComp's overlap event.
    // If your UPickupComponent is supposed to handle pickup detection instead,
    // you would need to move or adapt this logic, potentially binding to an
    // event within UPickupComponent itself.

    APawn* PlayerPawn = Cast<APawn>(OtherActor);
    if (PlayerPawn && OtherActor != this)
    {
        UE_LOG(LogTemp, Verbose, TEXT("PowerUp %s overlapped by Pawn %s (detected by CollisionComp)."), *GetName(), *PlayerPawn->GetName());
        Collected(PlayerPawn);
    }
}

// --- Collection Logic ---
void APowerUp::Collected(APawn* CollectorPawn)
{
     // 1. Apply the specific power-up effect to the collector.
     ApplyEffect(CollectorPawn);

     // 2. Notify the GameMode that this power-up was collected.
     ACakeChaseGameMode* GM = Cast<ACakeChaseGameMode>(GetWorld()->GetAuthGameMode());
     if (GM)
     {
         GM->NotifyPowerUpCollected(this->GetClass());
     }
     else
     {
         UE_LOG(LogTemp, Warning, TEXT("PowerUp %s could not get GameMode to notify collection."), *GetName());
     }

     // 3. Play Effects (Optional)
     // ...

     // 4. Destroy the power-up actor.
     Destroy();
}

// --- Default Effect Implementation ---
void APowerUp::ApplyEffect_Implementation(APawn* TargetPawn)
{
    UE_LOG(LogTemp, Warning, TEXT("ApplyEffect_Implementation called on base APowerUp class for %s. Target: %s. FORGOT TO OVERRIDE IN CHILD CLASS!"), *GetName(), *GetNameSafe(TargetPawn));
}
