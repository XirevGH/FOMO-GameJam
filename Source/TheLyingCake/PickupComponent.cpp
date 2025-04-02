// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupComponent.h"
#include "PlayerCharacter.h"
#include "EffectComponent.h"
#include "GameFramework/GameModeBase.h"


void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	UE_LOG(LogTemp,Warning, TEXT("Game mode type: %s"), *GetWorld()->GetAuthGameMode()->GetClass()->GetName());
	OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::OnPickup);
}

void UPickupComponent::ApplyEffectToActor(AActor* ActorToApply)
{
	UEffectComponent* Effect = NewObject<UEffectComponent>(ActorToApply, EffectClass);
	Effect->RegisterComponent();
	Effect->ApplyEffect(ActorToApply);
	

}


void UPickupComponent::OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Cast<APlayerCharacter>(OtherActor))
	{
		ApplyEffectToActor(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Player, %s, picked up the item "), *OtherActor->GetName());
		
	}
	GetOwner()->Destroy();
}
