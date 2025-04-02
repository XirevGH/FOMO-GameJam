// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h" // Include for FTimerHandle
#include "CakeChaseGameMode.generated.h"

class ACakePickup; // Forward declaration
class AMovementNode; // Forward declaration

UCLASS()
class THELYINGCAKE_API ACakeChaseGameMode : public AGameModeBase // Replace YOURPROJECTNAME_API
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // --- Configuration Properties (Editable in Blueprint Defaults) ---

    // The Blueprint class of the cake pickup to spawn. MUST be assigned in the Editor.
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<ACakePickup> CakeBP;

    // The Blueprint class of the Movement Node actor. MUST be assigned in the Editor.
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> MovementNodeBP; // Use AActor or your specific AMovementNode class

    // Minimum number of cakes to maintain in the world.
    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "0"))
    int32 MinimumCakes = 10;

    // Maximum number of cakes allowed in the world at once.
    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "1"))
    int32 MaximumCakes = 20;

    // How often (in seconds) to attempt spawning additional cakes.
    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "0.1"))
    float PeriodicSpawnInterval = 20.0f;

    // How many cakes to try and spawn during the periodic interval.
    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "1"))
    int32 PeriodicSpawnAmount = 2;


    // --- Internal State ---

    // Array to hold references to all found movement nodes.
    UPROPERTY() // Keep UPROPERTY to prevent garbage collection
    TArray<AActor*> MovementNodes;

    // Current count of active cakes in the world.
    UPROPERTY(VisibleInstanceOnly, Category = "Spawning") // Visible for debugging
    int32 CurrentAmountCakes = 0;

    // Timer handle for the periodic spawning logic.
    FTimerHandle PeriodicSpawnTimerHandle;


    // --- Spawning Logic Functions ---

    // Finds all MovementNode actors in the level.
    void FindAllMovementNodes();

    // Attempts to spawn a single cake at a random valid MovementNode location.
    // Returns true if successful, false otherwise.
    bool AttemptSpawnSingleCake();

    // Called periodically by the timer to spawn cakes.
    UFUNCTION() // Needs to be UFUNCTION to be used with timers
    void HandlePeriodicSpawn();

public:
    // Call this function from your ACakePickup when it gets collected/destroyed.
    UFUNCTION(BlueprintCallable, Category = "Game State") // Optional: Expose to BP if needed
    void NotifyCakeCollected();
    
    // Returns the current number of cakes in the level
    int32 GetCurrentCakeCount() const { return CurrentAmountCakes; }
    
    // Updates the cake count by directly counting all cake actors in the level
    void UpdateActualCakeCount();

};