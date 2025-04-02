#include "CakePickup.h"
#include "CakeChaseGameMode.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h" // Replace with your actual player character class name

ACakePickup::ACakePickup()
{
	PrimaryActorTick.bCanEverTick = false;
    
	// Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAll"));
	CollisionComponent->SetGenerateOverlapEvents(true);
    
	// Create the mesh component
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
    
	// Early exit if not collectable
	if (!bIsCollectable)
		return;
    
	// Check if the overlapping actor is the player character
	// Replace AYourPlayerCharacterClass with your actual player character class
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter)
		return;
    
	// Prevent multiple collections - set flag first
	bIsCollectable = false;
    
	// Notify the game mode about the collection
	ACakeChaseGameMode* GameMode = Cast<ACakeChaseGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->NotifyCakeCollected();
	}
    
	// Optional: Play effects, add score, etc.
	// ...
    
	// Destroy the cake - do this last
	Destroy();
}