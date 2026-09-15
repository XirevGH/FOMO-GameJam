#include "EffectComponent.h"
#include "TimerManager.h"
#include "PlayerCharacter.h"

UEffectComponent::UEffectComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}




void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void UEffectComponent::ApplyEffect(AActor* Actor)
{
	//this->DestroyComponent();
}



void UEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

