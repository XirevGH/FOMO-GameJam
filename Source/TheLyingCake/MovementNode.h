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

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Maze Node")
	TArray<AMovementNode*> ConnectedNodes;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Maze Node")
	class UBillboardComponent* EditorVisualizer;
	
protected:
	virtual void BeginPlay() override;
};
