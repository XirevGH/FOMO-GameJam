#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "CakeChaseGameMode.generated.h"

class ACakePickup;
class AMovementNode;

UCLASS()
class THELYINGCAKE_API ACakeChaseGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<ACakePickup> CakeBP;
    
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> MovementNodeBP;
    
    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "0"))
    int32 MinimumCakes = 10;
    
    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "1"))
    int32 MaximumCakes = 20;
    
    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "0.1"))
    float PeriodicSpawnInterval = 20.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "1"))
    int32 PeriodicSpawnAmount = 2;
    
    UPROPERTY()
    TArray<AActor*> MovementNodes;
    
    UPROPERTY(VisibleInstanceOnly, Category = "Spawning") 
    int32 CurrentAmountCakes = 0;
    
    FTimerHandle PeriodicSpawnTimerHandle;
    
    void FindAllMovementNodes();
    
    bool AttemptSpawnSingleCake();

    UFUNCTION()
    void HandlePeriodicSpawn();

public:
    UFUNCTION(BlueprintCallable, Category = "Game State")
    void NotifyCakeCollected();
    
    int32 GetCurrentCakeCount() const { return CurrentAmountCakes; }

    void UpdateActualCakeCount();

};