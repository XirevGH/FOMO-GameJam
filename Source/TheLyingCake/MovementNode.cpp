#include "MovementNode.h"

AMovementNode::AMovementNode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMovementNode::BeginPlay()
{
	Super::BeginPlay();

	FHitResult Hit;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 10000.f);
	FCollisionQueryParams Params;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		float DesiredHeight = Hit.Location.Z + 102.15f;
		FVector NewLocation = GetActorLocation();
		NewLocation.Z = DesiredHeight;
		SetActorLocation(NewLocation);
	}
	
}

