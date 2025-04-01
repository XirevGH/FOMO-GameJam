// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectComponent.h"
#include "TimerManager.h"


// Sets default values for this component's properties
UEffectComponent::UEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void UEffectComponent::ApplyEffect(AActor* Actor)
{
	Actor->SetActorRotation(FRotator(0,0,180));
	UE_LOG(LogTemp, Warning, TEXT("Do stuff to actor"));
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UEffectComponent::DestroyComponent, false);
	GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
}


// Called every frame
void UEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FRotator a = GetOwner()->GetActorRotation();
	
	GetOwner()->SetActorRotation(a + FRotator(0,0,1));
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

