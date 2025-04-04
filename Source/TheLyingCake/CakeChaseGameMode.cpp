// Copyright YourCompany. All Rights Reserved.

#include "CakeChaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "CakePickup.h"
#include "PowerUp.h" // Make sure to create and include your base power-up class header
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h" // Required for AActor

// --- BeginPlay ---
void ACakeChaseGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("CakeChaseGameMode BeginPlay started."));

    // --- Validation ---
    if (!CakeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Cake BP class is not assigned in the GameMode Blueprint!"));
        // Optionally return or handle gracefully
    }
    if (!MovementNodeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Movement Node BP class is not assigned in the GameMode Blueprint!"));
        return; // Cannot spawn anything without nodes
    }
    // Validate PowerUp Configurations (optional but recommended)
    for (const FPowerUpSpawnConfig& Config : PowerUpConfigs)
    {
        if (!Config.PowerUpClass)
        {
             UE_LOG(LogTemp, Error, TEXT("A PowerUpConfig entry is missing its PowerUpClass assignment in the GameMode Blueprint!"));
             // Decide how to handle this - skip this config, stop setup?
        }
    }


    // --- Initialization ---
    FindAllMovementNodes();

    if (MovementNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Movement Nodes found in the level. Cannot spawn cakes or power-ups."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Minimum Cakes: %d, Maximum Cakes: %d"), MinimumCakes, MaximumCakes);
    UE_LOG(LogTemp, Log, TEXT("Found %d PowerUp Configurations."), PowerUpConfigs.Num());

    // --- Initial Spawning ---
    // Spawn initial cakes
    UE_LOG(LogTemp, Log, TEXT("Spawning initial %d cakes..."), MinimumCakes);
    int32 SpawnedCakeCount = 0;
    // Add a safety limit to prevent infinite loops if spawning always fails
    const int32 MaxInitialSpawnAttempts = MinimumCakes * 3;
    while (CurrentAmountCakes < MinimumCakes && SpawnedCakeCount < MaxInitialSpawnAttempts)
    {
        AttemptSpawnSingleCake(); // This function now increments CurrentAmountCakes internally on success
        SpawnedCakeCount++;
    }
     if (CurrentAmountCakes < MinimumCakes)
    {
         UE_LOG(LogTemp, Warning, TEXT("Could not spawn all initial cakes after %d attempts. Spawned %d out of %d."), MaxInitialSpawnAttempts, CurrentAmountCakes, MinimumCakes);
    }
     else
    {
         UE_LOG(LogTemp, Log, TEXT("Initial cake spawn complete. Current cakes: %d"), CurrentAmountCakes);
     }

    // Initial Power-Up Counts (start at 0, they spawn periodically)
    for (FPowerUpSpawnConfig& Config : PowerUpConfigs) // Use reference to modify CurrentCount
    {
        Config.CurrentCount = 0; // Ensure counts start at zero
    }
    UpdateActualPowerUpCounts(); // Perform an initial count in case some were placed manually

    // --- Start Periodic Spawning Timer ---
    GetWorldTimerManager().SetTimer(
        PeriodicSpawnTimerHandle,
        this,
        &ACakeChaseGameMode::HandlePeriodicSpawn,
        PeriodicSpawnInterval,
        true, // Loop
        PeriodicSpawnInterval // Initial delay same as interval
    );

    UE_LOG(LogTemp, Log, TEXT("Periodic spawn timer started. Interval: %.2f seconds."), PeriodicSpawnInterval);
}

// --- Find Movement Nodes ---
void ACakeChaseGameMode::FindAllMovementNodes()
{
    UE_LOG(LogTemp, Log, TEXT("Finding all Movement Nodes..."));
    MovementNodes.Empty(); // Clear previous findings
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), MovementNodeBP, MovementNodes);
    UE_LOG(LogTemp, Log, TEXT("Found %d Movement Nodes."), MovementNodes.Num());
}

// --- Generic Actor Spawning ---
AActor* ACakeChaseGameMode::AttemptSpawnActorAtRandomNode(TSubclassOf<AActor> ActorClassToSpawn)
{
     if (!ActorClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("AttemptSpawnActorAtRandomNode called with null ActorClassToSpawn."));
        return nullptr;
    }

    if (MovementNodes.Num() == 0)
    {
       UE_LOG(LogTemp, Warning, TEXT("Attempted to spawn actor (%s), but no Movement Nodes available."), *ActorClassToSpawn->GetName());
       return nullptr;
    }

    // --- Find Available Nodes (Nodes without *any* actor overlapping nearby) ---
    TArray<AActor*> AvailableNodes;
    const float CheckRadius = 50.0f; // Radius around node center to check for obstructions
    TArray<AActor*> OverlappingActors; // Reused array for overlap checks
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // Empty = default object types
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
    // Add other relevant object types if needed

    for (AActor* Node : MovementNodes)
    {
        if (!Node) continue; // Skip invalid nodes

        FVector NodeLocation = Node->GetActorLocation();
        OverlappingActors.Empty(); // Clear for next check

        bool bIsOccupied = UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(),
            NodeLocation,
            CheckRadius,
            ObjectTypes,        // Object types to check against
            AActor::StaticClass(), // Class filter (any actor)
            TArray<AActor*>(),  // Actors to ignore (none)
            OverlappingActors   // Output array
        );

        if (!bIsOccupied) // If the sphere overlap *didn't* find any actors
        {
            AvailableNodes.Add(Node);
        }
        // Optional: Log if a node is occupied
        // else { UE_LOG(LogTemp, VeryVerbose, TEXT("Node at %s is occupied."), *NodeLocation.ToString()); }
    }

    // --- Select Node and Spawn ---
    if (AvailableNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No available (unoccupied) nodes found to spawn actor (%s)."), *ActorClassToSpawn->GetName());
        return nullptr; // No free spot
    }

    int32 RandomNodeIndex = FMath::RandRange(0, AvailableNodes.Num() - 1);
    AActor* ChosenNode = AvailableNodes[RandomNodeIndex];

    if (!ChosenNode)
    {
        // This shouldn't happen if AvailableNodes contains valid actors, but check anyway
        UE_LOG(LogTemp, Warning, TEXT("Selected available Movement Node at index %d is invalid."), RandomNodeIndex);
        return nullptr;
    }

    FVector Location = ChosenNode->GetActorLocation();
    FRotator Rotation = ChosenNode->GetActorRotation(); // Or FRotator::ZeroRotator if rotation doesn't matter

    // Spawn the actor
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClassToSpawn, Location, Rotation);

    if (SpawnedActor)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Successfully spawned actor %s at location %s."), *SpawnedActor->GetName(), *Location.ToString());
        return SpawnedActor;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor of class %s at location %s. Check collision settings or class validity."), *ActorClassToSpawn->GetName(), *Location.ToString());
        return nullptr;
    }
}


// --- Specific Cake Spawning (Uses Generic Spawner) ---
bool ACakeChaseGameMode::AttemptSpawnSingleCake()
{
    if (CurrentAmountCakes >= MaximumCakes)
    {
        // UE_LOG(LogTemp, Verbose, TEXT("Cannot spawn cake: Maximum cake limit reached (%d/%d)."), CurrentAmountCakes, MaximumCakes);
        return false; // Already at max
    }

    if (!CakeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot spawn cake: CakeBP is not set."));
        return false;
    }

    AActor* SpawnedActor = AttemptSpawnActorAtRandomNode(CakeBP);

    if (SpawnedActor)
    {
        CurrentAmountCakes++; // Increment count ONLY on successful spawn
        return true;
    }
    else
    {
        // AttemptSpawnActorAtRandomNode already logged the reason for failure
        return false;
    }
}


// --- Periodic Spawn Handler (Cakes & Power-Ups) ---
void ACakeChaseGameMode::HandlePeriodicSpawn()
{
    // --- Update Counts ---
    // It's good practice to refresh counts periodically in case actors were destroyed unexpectedly
    UpdateActualCakeCount();
    UpdateActualPowerUpCounts();

    UE_LOG(LogTemp, Verbose, TEXT("Handling periodic spawn. Current cakes: %d"), CurrentAmountCakes);

    // --- Cake Spawning Logic ---
    // 1. Ensure minimum cakes
    int32 CakesToSpawnForMin = FMath::Max(0, MinimumCakes - CurrentAmountCakes);
    if (CakesToSpawnForMin > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Below minimum cakes (%d/%d). Spawning %d to reach minimum."),
            CurrentAmountCakes, MinimumCakes, CakesToSpawnForMin);

        for (int32 i = 0; i < CakesToSpawnForMin; ++i)
        {
            if (!AttemptSpawnSingleCake())
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to spawn a cake while trying to reach minimum. Stopping cake spawn for this interval."));
                break; // Stop trying if one fails (e.g., no space)
            }
        }
        // Update count again after attempting to reach min, before proceeding
        UpdateActualCakeCount();
    }

    // 2. Add periodic cakes (up to maximum)
    int32 RemainingCapacity = MaximumCakes - CurrentAmountCakes;
    int32 CakesToAddPeriodically = FMath::Min(PeriodicSpawnAmount, RemainingCapacity);

    if (CakesToAddPeriodically > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Adding %d periodic cakes (Capacity: %d)."),
            CakesToAddPeriodically, RemainingCapacity);

        for (int32 i = 0; i < CakesToAddPeriodically; ++i)
        {
            if (!AttemptSpawnSingleCake())
            {
                 UE_LOG(LogTemp, Warning, TEXT("Failed to spawn a periodic cake. Stopping cake spawn for this interval."));
                break; // Stop trying if one fails
            }
        }
    }

    // --- Power-Up Spawning Logic ---
    UE_LOG(LogTemp, Verbose, TEXT("Checking power-up spawns..."));
    for (FPowerUpSpawnConfig& Config : PowerUpConfigs) // Use reference to update CurrentCount
    {
        if (!Config.PowerUpClass) continue; // Skip configs without a class assigned

        // Check 1: Is this power-up type below its max concurrent limit?
        if (Config.CurrentCount < Config.MaxConcurrent)
        {
            // Check 2: Does the random chance succeed?
            float RandomChance = FMath::FRand(); // Random float between 0.0 and 1.0
            if (RandomChance < Config.SpawnChancePerInterval)
            {
                UE_LOG(LogTemp, Log, TEXT("Attempting to spawn power-up '%s' (Chance: %.2f, Rolled: %.2f, Count: %d/%d)"),
                    *Config.PowerUpClass->GetName(), Config.SpawnChancePerInterval, RandomChance, Config.CurrentCount, Config.MaxConcurrent);

                // Attempt to spawn this specific power-up
                AActor* SpawnedPowerUp = AttemptSpawnActorAtRandomNode(Config.PowerUpClass);

                if (SpawnedPowerUp)
                {
                    Config.CurrentCount++; // Increment count for this specific power-up type
                    UE_LOG(LogTemp, Log, TEXT("Successfully spawned power-up '%s'. New count: %d"), *Config.PowerUpClass->GetName(), Config.CurrentCount);
                }
                else
                {
                     UE_LOG(LogTemp, Warning, TEXT("Failed to spawn power-up '%s'. No suitable location found or other spawn error."), *Config.PowerUpClass->GetName());
                     // Decide if you want to 'break' here or let other power-ups try spawning
                }
            }
             // Optional: Log if chance failed
             // else { UE_LOG(LogTemp, Verbose, TEXT("Spawn chance failed for '%s' (Rolled: %.2f, Needed < %.2f)"), *Config.PowerUpClass->GetName(), RandomChance, Config.SpawnChancePerInterval); }
        }
        // Optional: Log if max count reached
        // else { UE_LOG(LogTemp, Verbose, TEXT("Skipping spawn for '%s': Max concurrent count reached (%d/%d)."), *Config.PowerUpClass->GetName(), Config.CurrentCount, Config.MaxConcurrent); }
    }

    UE_LOG(LogTemp, Verbose, TEXT("Periodic spawn finished. Current cakes: %d"), CurrentAmountCakes);
    // You could add logging for final power-up counts here if needed
}

// --- Collection Notifications ---
void ACakeChaseGameMode::NotifyCakeCollected()
{
    int32 OldCount = CurrentAmountCakes;
    CurrentAmountCakes = FMath::Max(0, CurrentAmountCakes - 1); // Decrement and ensure not negative
    UE_LOG(LogTemp, Log, TEXT("Cake collected notification received. Cake count updated from %d to %d."), OldCount, CurrentAmountCakes);

    // Optional: Trigger an immediate spawn check if below minimum
    // Note: The periodic timer will handle this anyway, but this makes it more responsive.
    if (CurrentAmountCakes < MinimumCakes)
    {
         UE_LOG(LogTemp, Log, TEXT("Cake count below minimum after collection. Triggering immediate spawn check."));
         // Consider if calling HandlePeriodicSpawn directly is too heavy.
         // Maybe just attempt to spawn one cake immediately?
         AttemptSpawnSingleCake();
         // Or, simply rely on the next HandlePeriodicSpawn call.
    }
}

void ACakeChaseGameMode::NotifyPowerUpCollected(TSubclassOf<APowerUp> CollectedPowerUpClass)
{
    if (!CollectedPowerUpClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("NotifyPowerUpCollected called with null class."));
        return;
    }

    FPowerUpSpawnConfig* Config = FindPowerUpConfig(CollectedPowerUpClass);

    if (Config)
    {
        int32 OldCount = Config->CurrentCount;
        Config->CurrentCount = FMath::Max(0, Config->CurrentCount - 1); // Decrement and ensure not negative
        UE_LOG(LogTemp, Log, TEXT("Power-up '%s' collected notification received. Count updated from %d to %d."),
            *CollectedPowerUpClass->GetName(), OldCount, Config->CurrentCount);
    }
    else
    {
        // This might happen if the collected power-up wasn't spawned via a config (e.g., placed manually)
        // or if the class doesn't exactly match the one in the config array (e.g., child class collected).
        // You might need a more robust way to find the config if you use inheritance heavily here.
        // For now, just log a warning.
        UE_LOG(LogTemp, Warning, TEXT("NotifyPowerUpCollected: Could not find matching PowerUpConfig for class '%s'. Count not decremented."), *CollectedPowerUpClass->GetName());
        // As a fallback, you could force an update of all counts:
        // UpdateActualPowerUpCounts();
    }
}


// --- Count Update Functions ---
void ACakeChaseGameMode::UpdateActualCakeCount()
{
    if (!CakeBP) return; // Cannot count if BP is not set

    TArray<AActor*> AllCakes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), CakeBP, AllCakes); // Use the specific BP class for accuracy

    int32 PreviousCount = CurrentAmountCakes;
    CurrentAmountCakes = AllCakes.Num();

    if (PreviousCount != CurrentAmountCakes)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cake count corrected. Updated from %d to %d (actual count)."),
            PreviousCount, CurrentAmountCakes);
    }
}

void ACakeChaseGameMode::UpdateActualPowerUpCounts()
{
    UE_LOG(LogTemp, Verbose, TEXT("Updating actual power-up counts..."));
    TArray<AActor*> FoundPowerUps; // Reused array

    for (FPowerUpSpawnConfig& Config : PowerUpConfigs) // Use reference to update count
    {
        if (!Config.PowerUpClass) continue;

        FoundPowerUps.Empty(); // Clear for the next type
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), Config.PowerUpClass, FoundPowerUps);

        int32 PreviousCount = Config.CurrentCount;
        Config.CurrentCount = FoundPowerUps.Num();

        if (PreviousCount != Config.CurrentCount)
        {
             UE_LOG(LogTemp, Warning, TEXT("Power-up count corrected for '%s'. Updated from %d to %d (actual count)."),
                 *Config.PowerUpClass->GetName(), PreviousCount, Config.CurrentCount);
        }
    }
}

// --- Helper Function ---
FPowerUpSpawnConfig* ACakeChaseGameMode::FindPowerUpConfig(TSubclassOf<APowerUp> PowerUpClass)
{
     if (!PowerUpClass) return nullptr;

     for (FPowerUpSpawnConfig& Config : PowerUpConfigs)
     {
         // Check if the provided class is the same as, or a child of, the class in the config
         if (Config.PowerUpClass && PowerUpClass->IsChildOf(Config.PowerUpClass)) // Use IsChildOf for flexibility
         {
             return &Config; // Return pointer to the config struct
         }
     }
     return nullptr; // Not found
}
