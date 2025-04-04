// Copyright YourCompany. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CakeChaseGameMode.generated.h"

// Forward declarations
class ACakePickup;
class APowerUp; // Assuming you create a base class APowerUpBase

/**
 * Struct to hold spawn configuration for a specific power-up type.
 */
USTRUCT(BlueprintType)
struct FPowerUpSpawnConfig
{
    GENERATED_BODY()

    // The Blueprint class of the power-up to spawn.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUps")
    TSubclassOf<APowerUp> PowerUpClass;

    // Chance (0.0 to 1.0) to attempt spawning this power-up during each spawn interval.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUps", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float SpawnChancePerInterval = 0.1f;

    // Maximum number of this specific power-up allowed in the level concurrently.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUps", meta = (ClampMin = "0"))
    int32 MaxConcurrent = 1;

    // Internal tracking of the current count (not meant to be edited directly).
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerUps|Internal", Transient) // Transient means it won't be saved
    int32 CurrentCount = 0;
};


UCLASS()
class THELYINGCAKE_API ACakeChaseGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    // Called when a cake is collected by a player.
    void NotifyCakeCollected();

    // Called when a power-up is collected by a player.
    // You'll need to call this from your APowerUpBase::NotifyActorBeginOverlap or similar collection logic.
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void NotifyPowerUpCollected(TSubclassOf<APowerUp> CollectedPowerUpClass);

protected:
    virtual void BeginPlay() override;

    // --- Cake Spawning Properties ---

    // The Blueprint class for the cake pickup.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning|Cake")
    TSubclassOf<ACakePickup> CakeBP;

    // The Blueprint class representing spawn locations.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning|Setup")
    TSubclassOf<AActor> MovementNodeBP;

    // Minimum number of cakes that should be present in the level.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Cake", meta = (ClampMin = "0"))
    int32 MinimumCakes = 5;

    // Maximum number of cakes allowed in the level concurrently.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Cake", meta = (ClampMin = "1"))
    int32 MaximumCakes = 10;

    // How often the game attempts to spawn new cakes/power-ups (in seconds).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Timing", meta = (ClampMin = "0.1"))
    float PeriodicSpawnInterval = 5.0f;

    // How many *additional* cakes to try spawning each interval (if below MaximumCakes and above MinimumCakes).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Cake", meta = (ClampMin = "0"))
    int32 PeriodicSpawnAmount = 1;

    // --- Power-Up Spawning Properties ---

    // Array containing configurations for each type of power-up that can spawn.
    // Add elements to this array in the Blueprint editor for each power-up type.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|PowerUps")
    TArray<FPowerUpSpawnConfig> PowerUpConfigs;

    // --- Internal State ---

    // Timer handle for periodic spawning.
    FTimerHandle PeriodicSpawnTimerHandle;

    // List of all found movement nodes in the level.
    UPROPERTY() // UPROPERTY prevents garbage collection
    TArray<AActor*> MovementNodes;

    // Current number of cakes present in the level.
    int32 CurrentAmountCakes = 0;

    // --- Spawning Logic ---

    // Finds all actors of MovementNodeBP type in the level.
    void FindAllMovementNodes();

    // Attempts to spawn a single cake at a random, unoccupied movement node.
    // Returns true if successful, false otherwise.
    bool AttemptSpawnSingleCake();

    // Attempts to spawn a single instance of the specified Actor class at a random, unoccupied movement node.
    // Returns the spawned actor if successful, nullptr otherwise.
    AActor* AttemptSpawnActorAtRandomNode(TSubclassOf<AActor> ActorClassToSpawn);

    // Called periodically by the timer to handle spawning cakes and power-ups.
    void HandlePeriodicSpawn();

    // Updates the internal count of cakes based on actors currently in the world.
    void UpdateActualCakeCount();

    // Updates the internal counts of power-ups based on actors currently in the world.
    void UpdateActualPowerUpCounts();

    // Helper to find the configuration struct for a given power-up class.
    FPowerUpSpawnConfig* FindPowerUpConfig(TSubclassOf<APowerUp> PowerUpClass);
};
