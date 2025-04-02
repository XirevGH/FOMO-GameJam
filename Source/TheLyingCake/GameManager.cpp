// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	
	HUD = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameManager::AddScoreToPlayer(const int Score, AActor* Player)
{
	if (Player != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Player);
		if (PlayerCharacter != nullptr)
		{
			int CurrentScore = PlayerCharacter->GetScore();
			PlayerCharacter->SetScore(CurrentScore + Score);
		}
	}
}


