// Copyright YourCompany. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CakeChaseGameMode.generated.h"

class ACakePickup;
class APowerUp;

USTRUCT(BlueprintType)
struct FPowerUpSpawnConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUps")
    TSubclassOf<APowerUp> PowerUpClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUps", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float SpawnChancePerInterval = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUps", meta = (ClampMin = "0"))
    int32 MaxConcurrent = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerUps|Internal", Transient) // Transient means it won't be saved
    int32 CurrentCount = 0;
};


UCLASS()
class THELYINGCAKE_API ACakeChaseGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    void NotifyCakeCollected();

    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void NotifyPowerUpCollected(TSubclassOf<APowerUp> CollectedPowerUpClass);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning|Cake")
    TSubclassOf<ACakePickup> CakeBP;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning|Setup")
    TSubclassOf<AActor> MovementNodeBP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Cake", meta = (ClampMin = "0"))
    int32 MinimumCakes = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Cake", meta = (ClampMin = "1"))
    int32 MaximumCakes = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Timing", meta = (ClampMin = "0.1"))
    float PeriodicSpawnInterval = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Cake", meta = (ClampMin = "0"))
    int32 PeriodicSpawnAmount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|PowerUps")
    TArray<FPowerUpSpawnConfig> PowerUpConfigs;

    FTimerHandle PeriodicSpawnTimerHandle;


    UPROPERTY()
    TArray<AActor*> MovementNodes;

    int32 CurrentAmountCakes = 0;

    void FindAllMovementNodes();

    bool AttemptSpawnSingleCake();

    AActor* AttemptSpawnActorAtRandomNode(TSubclassOf<AActor> ActorClassToSpawn);

    void HandlePeriodicSpawn();

    void UpdateActualCakeCount();

    void UpdateActualPowerUpCounts();

    FPowerUpSpawnConfig* FindPowerUpConfig(TSubclassOf<APowerUp> PowerUpClass);

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
    float TimerTime = 180.f;
};
