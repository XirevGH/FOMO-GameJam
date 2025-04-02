// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupComponent.h"
#include "PlayerCharacter.h"
#include "EffectComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	UE_LOG(LogTemp,Warning, TEXT("Pick up is being made"));
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
		UE_LOG(LogTemp,Warning, TEXT("Pick up is getting picked "));
		UE_LOG(LogTemp, Warning, TEXT("Player, %s, picked up the item "), *OtherActor->GetName());
		
	}
	if (PicUpParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, PicUpParticle, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	}
	GetOwner()->Destroy();
}
