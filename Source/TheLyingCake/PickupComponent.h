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
	virtual void BeginPlay() override;

protected:
	bool bHasBeenCollected;
	
	UPROPERTY(EditAnywhere, Category = "Pickup")
	TSubclassOf<UEffectComponent> EffectClass;
	
	UPROPERTY(EditAnywhere, Category = "Pickup")
	class UParticleSystem* PicUpParticle;
	
	void ApplyEffectToActor(AActor* ActorToApply);
	
	UFUNCTION()
	void OnPickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};