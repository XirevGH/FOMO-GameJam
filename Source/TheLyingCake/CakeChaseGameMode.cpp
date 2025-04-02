// Fill out your copyright notice in the Description page of Project Settings.

#include "CakeChaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "CakePickup.h"
#include "MovementNode.h" // Include your actual MovementNode header if it exists
#include "Engine/World.h" // Required for GetWorld()
#include "TimerManager.h" // Required for GetWorldTimerManager()

void ACakeChaseGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("CakeChaseGameMode BeginPlay started."));

    // --- Pre-Spawn Checks ---
    if (!CakeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Cake BP class is not assigned in the GameMode Blueprint!"));
        return; // Cannot spawn cakes without a class
    }
    if (!MovementNodeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Movement Node BP class is not assigned in the GameMode Blueprint!"));
        return; // Cannot find spawn locations without a class
    }

    // --- Initialization ---
    FindAllMovementNodes();

    if (MovementNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Movement Nodes found in the level. Cannot spawn cakes."));
        return; // No locations to spawn at
    }

    // Clamp Max cakes to be at least Min cakes
    MaximumCakes = FMath::Max(MinimumCakes, MaximumCakes);
    UE_LOG(LogTemp, Log, TEXT("Minimum Cakes: %d, Maximum Cakes: %d"), MinimumCakes, MaximumCakes);


    // --- Initial Spawn ---
    UE_LOG(LogTemp, Log, TEXT("Spawning initial %d cakes..."), MinimumCakes);
    int32 SpawnedCount = 0;
    while (CurrentAmountCakes < MinimumCakes && SpawnedCount < MinimumCakes * 2) // Add safety break
    {
        if (AttemptSpawnSingleCake())
        {
             // CurrentAmountCakes is incremented inside AttemptSpawnSingleCake on success
        }
        SpawnedCount++; // Prevent infinite loop if spawning fails repeatedly
    }
     if (CurrentAmountCakes < MinimumCakes)
    {
         UE_LOG(LogTemp, Warning, TEXT("Could not spawn all initial cakes. Spawned %d out of %d."), CurrentAmountCakes, MinimumCakes);
    }
     else
    {
         UE_LOG(LogTemp, Log, TEXT("Initial spawn complete. Current cakes: %d"), CurrentAmountCakes);
     }


    // --- Periodic Spawning Setup ---
    // Use a looping timer for periodic checks and spawning.
    GetWorldTimerManager().SetTimer(
        PeriodicSpawnTimerHandle,      // The handle to manage this timer
        this,                          // The object instance to call the function on
        &ACakeChaseGameMode::HandlePeriodicSpawn, // The function to call
        PeriodicSpawnInterval,         // How often to call (in seconds)
        true,                          // Should the timer loop? Yes.
        PeriodicSpawnInterval          // Initial delay before the first call (optional, can be 0 or omitted to start immediately)
    );

    UE_LOG(LogTemp, Log, TEXT("Periodic spawn timer started. Interval: %.2f seconds."), PeriodicSpawnInterval);
}

void ACakeChaseGameMode::FindAllMovementNodes()
{
    UE_LOG(LogTemp, Log, TEXT("Finding all Movement Nodes..."));
    // Clear previous findings if any (though usually called only once)
    MovementNodes.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), MovementNodeBP, MovementNodes);
    UE_LOG(LogTemp, Log, TEXT("Found %d Movement Nodes."), MovementNodes.Num());
}

bool ACakeChaseGameMode::AttemptSpawnSingleCake()
{
    if (CurrentAmountCakes >= MaximumCakes)
    {
        //UE_LOG(LogTemp, Verbose, TEXT("Maximum cake limit (%d) reached. Cannot spawn more."), MaximumCakes);
        return false; // Don't spawn if already at max
    }

    if (MovementNodes.Num() == 0)
    {
       UE_LOG(LogTemp, Warning, TEXT("Attempted to spawn cake, but no Movement Nodes available."));
       return false;
    }

    // Select a random node
    int32 RandomNodeIndex = FMath::RandRange(0, MovementNodes.Num()-1);
    AActor* ChosenNode = MovementNodes[RandomNodeIndex];

    if (!ChosenNode) // Safety check
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected Movement Node at index %d is invalid."), RandomNodeIndex);
        return false;
    }

    FVector Location = ChosenNode->GetActorLocation();
    FRotator Rotation = ChosenNode->GetActorRotation(); // Or FRotator::ZeroRotator if orientation doesn't matter
    

    // Spawn the cake
    ACakePickup* SpawnedCake = GetWorld()->SpawnActor<ACakePickup>(CakeBP, Location, Rotation);

    if (SpawnedCake)
    {
        //UE_LOG(LogTemp, Log, TEXT("Successfully spawned cake %s."), *SpawnedCake->GetName());
        CurrentAmountCakes++;
        // Optional: You might want the cake to know about the GameMode too, or bind to its destruction event here.
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn ACakePickup actor at location %s. Check collision or CakeBP validity."), *(Location).ToString());
        return false;
    }
}

void ACakeChaseGameMode::HandlePeriodicSpawn()
{
    //UE_LOG(LogTemp, Verbose, TEXT("Handling periodic spawn. Current cakes: %d"), CurrentAmountCakes);

    // 1. Ensure minimum is met
    int32 CakesToSpawnForMin = FMath::Max(0, MinimumCakes - CurrentAmountCakes);
    if (CakesToSpawnForMin > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Below minimum cakes (%d/%d). Spawning %d to reach minimum."), CurrentAmountCakes, MinimumCakes, CakesToSpawnForMin);
        for (int32 i = 0; i < CakesToSpawnForMin; ++i)
        {
            if (!AttemptSpawnSingleCake())
            {
                 // Stop trying if we hit the max limit or another issue occurs
                 break;
            }
        }
    }

    // 2. Add the periodic amount, respecting the maximum
    int32 RemainingCapacity = MaximumCakes - CurrentAmountCakes;
    int32 CakesToAddPeriodically = FMath::Min(PeriodicSpawnAmount, RemainingCapacity);

    if (CakesToAddPeriodically > 0)
    {
        // UE_LOG(LogTemp, Log, TEXT("Adding %d periodic cakes (Capacity: %d)."), CakesToAddPeriodically, RemainingCapacity);
         for (int32 i = 0; i < CakesToAddPeriodically; ++i)
         {
            if (!AttemptSpawnSingleCake())
            {
                 // Stop trying if we hit the max limit or another issue occurs
                 break;
            }
         }
    }
     // else { UE_LOG(LogTemp, Verbose, TEXT("No capacity to add periodic cakes or periodic amount is 0.")); }

    // UE_LOG(LogTemp, Verbose, TEXT("Periodic spawn finished. Current cakes: %d"), CurrentAmountCakes);
}

void ACakeChaseGameMode::NotifyCakeCollected()
{
    CurrentAmountCakes = FMath::Max(0, CurrentAmountCakes - 1); // Decrement, ensuring it doesn't go below 0
    UE_LOG(LogTemp, Log, TEXT("Cake collected. Current cakes: %d"), CurrentAmountCakes);

    // Immediately check if we need to spawn to maintain the minimum
    if (CurrentAmountCakes < MinimumCakes)
    {
        UE_LOG(LogTemp, Log, TEXT("Below minimum after collection. Triggering immediate spawn check."));
        // We can directly call HandlePeriodicSpawn as it contains the logic
        // to top up to the minimum amount first.
        HandlePeriodicSpawn();
        // Alternatively, you could have a simpler function here that just calls
        // AttemptSpawnSingleCake until the minimum is met, but reusing HandlePeriodicSpawn is fine.
    }
}