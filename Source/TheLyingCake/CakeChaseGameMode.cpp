#include "CakeChaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "CakePickup.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

void ACakeChaseGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("CakeChaseGameMode BeginPlay started."));
    
    if (!CakeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Cake BP class is not assigned in the GameMode Blueprint!"));
        return;
    }
    if (!MovementNodeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Movement Node BP class is not assigned in the GameMode Blueprint!"));
        return;
    }
    
    FindAllMovementNodes();

    if (MovementNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Movement Nodes found in the level. Cannot spawn cakes."));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Minimum Cakes: %d, Maximum Cakes: %d"), MinimumCakes, MaximumCakes);
    
    UE_LOG(LogTemp, Log, TEXT("Spawning initial %d cakes..."), MinimumCakes);
    int32 SpawnedCount = 0;
    while (CurrentAmountCakes < MinimumCakes && SpawnedCount < MinimumCakes * 2)
    {
        AttemptSpawnSingleCake();
        SpawnedCount++;
    }
     if (CurrentAmountCakes < MinimumCakes)
    {
         UE_LOG(LogTemp, Warning, TEXT("Could not spawn all initial cakes. Spawned %d out of %d."), CurrentAmountCakes, MinimumCakes);
    }
     else
    {
         UE_LOG(LogTemp, Log, TEXT("Initial spawn complete. Current cakes: %d"), CurrentAmountCakes);
     }
    
    GetWorldTimerManager().SetTimer(
        PeriodicSpawnTimerHandle,
        this,
        &ACakeChaseGameMode::HandlePeriodicSpawn,
        PeriodicSpawnInterval,
        true,
        PeriodicSpawnInterval
    );

    UE_LOG(LogTemp, Log, TEXT("Periodic spawn timer started. Interval: %.2f seconds."), PeriodicSpawnInterval);
}

void ACakeChaseGameMode::FindAllMovementNodes()
{
    UE_LOG(LogTemp, Log, TEXT("Finding all Movement Nodes..."));
    MovementNodes.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), MovementNodeBP, MovementNodes);
    UE_LOG(LogTemp, Log, TEXT("Found %d Movement Nodes."), MovementNodes.Num());
}

bool ACakeChaseGameMode::AttemptSpawnSingleCake()
{
    if (CurrentAmountCakes >= MaximumCakes)
    {
        return false;
    }

    if (MovementNodes.Num() == 0)
    {
       UE_LOG(LogTemp, Warning, TEXT("Attempted to spawn cake, but no Movement Nodes available."));
       return false;
    }
    
    TArray<AActor*> AvailableNodes;
    
    for (AActor* Node : MovementNodes)
    {
        if (!Node) continue;
        
        FVector NodeLocation = Node->GetActorLocation();
        bool bNodeOccupied = false;
        
        TArray<AActor*> OverlappingActors;

        const float CheckRadius = 50.0f; 
        UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(),
            NodeLocation,
            CheckRadius,
            TArray<TEnumAsByte<EObjectTypeQuery>>(),
            AActor::StaticClass(),
            TArray<AActor*>(),
            OverlappingActors
        );
        
        if (OverlappingActors.Num() == 0)
        {
            AvailableNodes.Add(Node);
        }
    }
    
    if (AvailableNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No available nodes to spawn cakes - all nodes occupied."));
        return false;
    }
    
    int32 RandomNodeIndex = FMath::RandRange(0, AvailableNodes.Num()-1);
    AActor* ChosenNode = AvailableNodes[RandomNodeIndex];

    if (!ChosenNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected available Movement Node at index %d is invalid."), RandomNodeIndex);
        return false;
    }

    FVector Location = ChosenNode->GetActorLocation();
    FRotator Rotation = ChosenNode->GetActorRotation();
    
    ACakePickup* SpawnedCake = GetWorld()->SpawnActor<ACakePickup>(CakeBP, Location, Rotation);

    if (SpawnedCake)
    {
        CurrentAmountCakes++;
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn ACakePickup actor at location %s. Check collision or CakeBP validity."), *(Location).ToString());
        return false;
    }
}

void ACakeChaseGameMode::NotifyCakeCollected()
{
    CurrentAmountCakes = FMath::Max(0, CurrentAmountCakes - 1);
    UE_LOG(LogTemp, Log, TEXT("Cake collected. Current cakes: %d"), CurrentAmountCakes);
    
    if (CurrentAmountCakes < MinimumCakes)
    {
        UE_LOG(LogTemp, Log, TEXT("Below minimum after collection. Triggering immediate spawn check."));
        HandlePeriodicSpawn();
    }
}

void ACakeChaseGameMode::UpdateActualCakeCount()
{
    TArray<AActor*> AllCakes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACakePickup::StaticClass(), AllCakes);
    
    int32 PreviousCount = CurrentAmountCakes;
    CurrentAmountCakes = AllCakes.Num();
    
    if (PreviousCount != CurrentAmountCakes)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cake count was incorrect. Updated from %d to %d (actual count)."), 
            PreviousCount, CurrentAmountCakes);
    }
}

void ACakeChaseGameMode::HandlePeriodicSpawn()
{
    UpdateActualCakeCount();
    UE_LOG(LogTemp, Verbose, TEXT("Handling periodic spawn. Current cakes: %d"), CurrentAmountCakes);
    
    int32 CakesToSpawnForMin = FMath::Max(0, MinimumCakes - CurrentAmountCakes);
    if (CakesToSpawnForMin > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Below minimum cakes (%d/%d). Spawning %d to reach minimum."), 
            CurrentAmountCakes, MinimumCakes, CakesToSpawnForMin);
            
        for (int32 i = 0; i < CakesToSpawnForMin; ++i)
        {
            if (!AttemptSpawnSingleCake())
            {
                break;
            }
        }
    }
    
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
                break;
            }
        }
    }
    
    UE_LOG(LogTemp, Verbose, TEXT("Periodic spawn finished. Current cakes: %d"), CurrentAmountCakes);
}
