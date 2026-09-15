#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPickupComponent;
class APawn;

UCLASS(Abstract, Blueprintable)
class THELYINGCAKE_API APowerUp : public AActor
{
    GENERATED_BODY()

public:
    APowerUp();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> CollisionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UPickupComponent> PickupComp;

    UFUNCTION()
    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    virtual void Collected(APawn* CollectorPawn);

    UFUNCTION(BlueprintNativeEvent, Category = "PowerUp")
    void ApplyEffect(APawn* TargetPawn);

    virtual void ApplyEffect_Implementation(APawn* TargetPawn);
};
