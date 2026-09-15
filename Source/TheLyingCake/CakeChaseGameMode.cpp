// Copyright YourCompany. All Rights Reserved.

#include "CakeChaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "CakePickup.h"
#include "PowerUp.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

void ACakeChaseGameMode::BeginPlay()
{
    Super::BeginPlay();

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
        UE_LOG(LogTemp, Warning, TEXT("No Movement Nodes found in the level. Cannot spawn cakes or power-ups."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Minimum Cakes: %d, Maximum Cakes: %d"), MinimumCakes, MaximumCakes);
    UE_LOG(LogTemp, Log, TEXT("Found %d PowerUp Configurations."), PowerUpConfigs.Num());

    UE_LOG(LogTemp, Log, TEXT("Spawning initial %d cakes..."), MinimumCakes);
    int32 SpawnedCakeCount = 0;

    const int32 MaxInitialSpawnAttempts = MinimumCakes * 3;
    while (CurrentAmountCakes < MinimumCakes && SpawnedCakeCount < MaxInitialSpawnAttempts)
    {
        AttemptSpawnSingleCake();
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


    for (FPowerUpSpawnConfig& Config : PowerUpConfigs)
    {
        Config.CurrentCount = 0;
    }
    UpdateActualPowerUpCounts();

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

    TArray<AActor*> AvailableNodes;
    const float CheckRadius = 50.0f;
    TArray<AActor*> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

    for (AActor* Node : MovementNodes)
    {
        if (!Node) continue;

        FVector NodeLocation = Node->GetActorLocation();
        OverlappingActors.Empty();

        bool bIsOccupied = UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(),
            NodeLocation,
            CheckRadius,
            ObjectTypes,
            AActor::StaticClass(),
            TArray<AActor*>(),
            OverlappingActors
        );

        if (!bIsOccupied)
        {
            AvailableNodes.Add(Node);
        }
    }

    if (AvailableNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No available (unoccupied) nodes found to spawn actor (%s)."), *ActorClassToSpawn->GetName());
        return nullptr;
    }

    int32 RandomNodeIndex = FMath::RandRange(0, AvailableNodes.Num() - 1);
    AActor* ChosenNode = AvailableNodes[RandomNodeIndex];

    if (!ChosenNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected available Movement Node at index %d is invalid."), RandomNodeIndex);
        return nullptr;
    }

    FVector Location = ChosenNode->GetActorLocation();
    FRotator Rotation = ChosenNode->GetActorRotation();

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

bool ACakeChaseGameMode::AttemptSpawnSingleCake()
{
    if (CurrentAmountCakes >= MaximumCakes)
    {
        return false;
    }

    if (!CakeBP)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot spawn cake: CakeBP is not set."));
        return false;
    }

    AActor* SpawnedActor = AttemptSpawnActorAtRandomNode(CakeBP);

    if (SpawnedActor)
    {
        CurrentAmountCakes++;
        return true;
    }
    else
    {
        return false;
    }
}

void ACakeChaseGameMode::HandlePeriodicSpawn()
{
    UpdateActualCakeCount();
    UpdateActualPowerUpCounts();

    UE_LOG(LogTemp, Verbose, TEXT("Handling periodic spawn. Current cakes: %d"), CurrentAmountCakes);

    int32 CakesToSpawnForMin = FMath::Max(0, MinimumCakes - CurrentAmountCakes);
    if (CakesToSpawnForMin > 0)
    {
            CurrentAmountCakes, MinimumCakes, CakesToSpawnForMin);

        for (int32 i = 0; i < CakesToSpawnForMin; ++i)
        {
            if (!AttemptSpawnSingleCake())
            {
                break;
            }
        }
        UpdateActualCakeCount();
    }

    int32 RemainingCapacity = MaximumCakes - CurrentAmountCakes;
    int32 CakesToAddPeriodically = FMath::Min(PeriodicSpawnAmount, RemainingCapacity);

    if (CakesToAddPeriodically > 0)
    {
            CakesToAddPeriodically, RemainingCapacity);

        for (int32 i = 0; i < CakesToAddPeriodically; ++i)
        {
            if (!AttemptSpawnSingleCake())
            {
                break;
            }
        }
    }

    UE_LOG(LogTemp, Verbose, TEXT("Checking power-up spawns..."));
    for (FPowerUpSpawnConfig& Config : PowerUpConfigs)
    {
        if (!Config.PowerUpClass) continue;

        if (Config.CurrentCount < Config.MaxConcurrent)
        {
            float RandomChance = FMath::FRand();
            if (RandomChance < Config.SpawnChancePerInterval)
            {
                UE_LOG(LogTemp, Log, TEXT("Attempting to spawn power-up '%s' (Chance: %.2f, Rolled: %.2f, Count: %d/%d)"),
                    *Config.PowerUpClass->GetName(), Config.SpawnChancePerInterval, RandomChance, Config.CurrentCount, Config.MaxConcurrent);

                AActor* SpawnedPowerUp = AttemptSpawnActorAtRandomNode(Config.PowerUpClass);

                if (SpawnedPowerUp)
                {
                    Config.CurrentCount++;
                    UE_LOG(LogTemp, Log, TEXT("Successfully spawned power-up '%s'. New count: %d"), *Config.PowerUpClass->GetName(), Config.CurrentCount);
                }
            }
        }
    }
}

void ACakeChaseGameMode::NotifyCakeCollected()
{
    int32 OldCount = CurrentAmountCakes;
    CurrentAmountCakes = FMath::Max(0, CurrentAmountCakes - 1);

    if (CurrentAmountCakes < MinimumCakes)
    {
         AttemptSpawnSingleCake();
    }
}

void ACakeChaseGameMode::NotifyPowerUpCollected(TSubclassOf<APowerUp> CollectedPowerUpClass)
{
    if (!CollectedPowerUpClass)
    {
        return;
    }

    FPowerUpSpawnConfig* Config = FindPowerUpConfig(CollectedPowerUpClass);

    if (Config)
    {
        int32 OldCount = Config->CurrentCount;
        Config->CurrentCount = FMath::Max(0, Config->CurrentCount - 1);
    }
}

void ACakeChaseGameMode::UpdateActualCakeCount()
{
    if (!CakeBP) return;

    TArray<AActor*> AllCakes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), CakeBP, AllCakes);

    int32 PreviousCount = CurrentAmountCakes;
    CurrentAmountCakes = AllCakes.Num();
}

void ACakeChaseGameMode::UpdateActualPowerUpCounts()
{
    TArray<AActor*> FoundPowerUps;

    for (FPowerUpSpawnConfig& Config : PowerUpConfigs) 
    {
        if (!Config.PowerUpClass) continue;

        FoundPowerUps.Empty();
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), Config.PowerUpClass, FoundPowerUps);

        int32 PreviousCount = Config.CurrentCount;
        Config.CurrentCount = FoundPowerUps.Num();
    }
}

FPowerUpSpawnConfig* ACakeChaseGameMode::FindPowerUpConfig(TSubclassOf<APowerUp> PowerUpClass)
{
     if (!PowerUpClass) return nullptr;

     for (FPowerUpSpawnConfig& Config : PowerUpConfigs)
     {
         if (Config.PowerUpClass && PowerUpClass->IsChildOf(Config.PowerUpClass))
         {
             return &Config;
         }
     }
     return nullptr;
}
