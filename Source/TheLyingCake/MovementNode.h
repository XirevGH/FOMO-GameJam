#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementNode.generated.h"

UCLASS()
class THELYINGCAKE_API AMovementNode : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovementNode();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Movement Node")
	TArray<AMovementNode*> ConnectedNodes;
	
protected:
	virtual void BeginPlay() override;
};
