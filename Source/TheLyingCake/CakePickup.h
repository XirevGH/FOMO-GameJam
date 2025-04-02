// Fill out your copyright notice in the Description page of Project Settings.

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
    
	// Override the default collision function
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    
protected:
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bIsCollectable;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class USphereComponent* CollisionComponent;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class UStaticMeshComponent* CakeMeshComponent;
    
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

