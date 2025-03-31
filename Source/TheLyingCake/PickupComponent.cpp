// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupComponent.h"

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::OnPickup);
}

void UPickupComponent::ApplyEffectToActor(AActor* ActorToApply)
{
	UE_LOG(LogTemp, Warning, TEXT("Do stuff to actor, %s"), *ActorToApply->GetName());
}


void UPickupComponent::OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AActor::StaticClass()))
	{
		ApplyEffectToActor(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Player picked up the item %s"), *OtherActor->GetName());
		
	}
	GetOwner()->Destroy();
}
