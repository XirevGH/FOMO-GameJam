// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

UCLASS()
class THELYINGCAKE_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void AddScoreToPlayer(const int Score, AActor* Player);

private:
	UPROPERTY(EditDefaultsOnly)
	int ScoreToWin{5};
	UPROPERTY(EditAnywhere)
	AActor* PlayerOne;
	UPROPERTY(EditAnywhere)
	AActor* PlayerTwo;

	UPROPERTY(EditAnywhere)
	TSubclassOf< UUserWidget> HUDClass;
	UPROPERTY()
	UUserWidget* HUD;
	
};
