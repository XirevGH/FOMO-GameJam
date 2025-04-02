#include "CakePickup.h"
#include "CakeChaseGameMode.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ACakePickup::ACakePickup()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAll"));
	CollisionComponent->SetGenerateOverlapEvents(true);
	
	CakeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CakeMeshComponent"));
	CakeMeshComponent->SetupAttachment(RootComponent);
	CakeMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
    
	bIsCollectable = true;
}

void ACakePickup::BeginPlay()
{
	Super::BeginPlay();
}

void ACakePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!bIsCollectable)
		return;
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter)
		return;
	
	bIsCollectable = false;
	
	ACakeChaseGameMode* GameMode = Cast<ACakeChaseGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->NotifyCakeCollected();
	}
	//TODO: Add pickup sound
	Destroy();
}