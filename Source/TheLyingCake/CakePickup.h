#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CakePickup.generated.h"

UCLASS()
class THELYINGCAKE_API ACakePickup : public AActor
{
	GENERATED_BODY()
    
public:
	ACakePickup();
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bIsCollectable;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class USphereComponent* CollisionComponent;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class UStaticMeshComponent* CakeMeshComponent;
	
	virtual void BeginPlay() override;
};

