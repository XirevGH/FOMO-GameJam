// Fill out your copyright notice in the Description page of Project Settings.


#include "CakePickup.h"

// Sets default values
ACakePickup::ACakePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACakePickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACakePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

