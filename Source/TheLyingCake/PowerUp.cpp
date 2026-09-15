// Copyright YourCompany. All Rights Reserved.

#include "PowerUp.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "CakeChaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PickupComponent.h"


APowerUp::APowerUp()
{
    PrimaryActorTick.bCanEverTick = false;

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


    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    if (MeshComp && RootComponent)
    {
        MeshComp->SetupAttachment(RootComponent);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
     else
    {
         if (!MeshComp) UE_LOG(LogTemp, Error, TEXT("APowerUp: Failed to create MeshComp!"));
    }

    PickupComp = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComp"));
    if (PickupComp && MeshComp)
    {
        PickupComp->SetupAttachment(MeshComp);
    }
}

void APowerUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    APawn* PlayerPawn = Cast<APawn>(OtherActor);
    if (PlayerPawn && OtherActor != this)
    {
        UE_LOG(LogTemp, Verbose, TEXT("PowerUp %s overlapped by Pawn %s (detected by CollisionComp)."), *GetName(), *PlayerPawn->GetName());
        Collected(PlayerPawn);
    }
}

void APowerUp::Collected(APawn* CollectorPawn)
{
     ApplyEffect(CollectorPawn);

     ACakeChaseGameMode* GM = Cast<ACakeChaseGameMode>(GetWorld()->GetAuthGameMode());
     if (GM)
     {
         GM->NotifyPowerUpCollected(this->GetClass());
     }

     Destroy();
}

void APowerUp::ApplyEffect_Implementation(APawn* TargetPawn)
{

}
