#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

// Forward declarations
class USphereComponent;
class UStaticMeshComponent;
class UPickupComponent; // Forward declare your custom component
class APawn;

UCLASS(Abstract, Blueprintable) // Abstract: Cannot spawn this base class directly. Blueprintable: Can create Blueprints inheriting from this.
class THELYINGCAKE_API APowerUp : public AActor // Replace YOURPROJECTNAME_API with your project's API macro
{
    GENERATED_BODY()

public:
    // Constructor
    APowerUp();

protected:
    // --- Components ---

    // Sphere component used for collision detection and triggering overlap events (pickup).
    // This will be the Root Component.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> CollisionComp;

    // Static mesh component to represent the power-up visually.
    // This will be attached to the CollisionComp.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> MeshComp;

    // Your custom Pickup Component.
    // This will be attached to the MeshComp.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UPickupComponent> PickupComp; // Added your component

    // --- Overlap & Collection Logic ---

    // Called when an actor begins overlapping the CollisionComp.
    // We'll use UFUNCTION to bind it dynamically.
    UFUNCTION()
    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Function to handle the actual collection logic (called by OnOverlapBegin).
    // Marked virtual so subclasses can potentially override collection behavior if needed.
    virtual void Collected(APawn* CollectorPawn);

    // --- Power-Up Effect ---

    // Function to apply the specific power-up effect to the target pawn.
    // BlueprintNativeEvent allows implementation in Blueprint or C++.
    // Needs to be implemented by derived classes (Blueprint or C++).
    UFUNCTION(BlueprintNativeEvent, Category = "PowerUp")
    void ApplyEffect(APawn* TargetPawn);
    // C++ implementation function declaration (required for BlueprintNativeEvent)
    virtual void ApplyEffect_Implementation(APawn* TargetPawn);

};
