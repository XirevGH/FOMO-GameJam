#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputAction;
class UCameraComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;
class AMovementNode;

UCLASS()
class THELYINGCAKE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateRightAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateLeftAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateAroundAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveForwardAction;
	
public:
	APlayerCharacter();

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	
	UFUNCTION(BlueprintCallable)
	int GetScore() const { return Score; }
	
	void SetScore(const int ScoreToSet) {  Score = ScoreToSet; }
	
	float GetMovementSpeed() const { return MovementSpeed; }
	void SetMovementSpeed(const float Speed) {  MovementSpeed = Speed; }
	float GetBaseMovementSpeed() const { return BaseMovementSpeed; }
	void SetSlowAmount(float S) { SlowAmount = S; }
	void SetSpeedwAmount(float S) { SpeedAmount = S; }
	AMovementNode* GetCurrentNode() const { return CurrentNode; }
	void SetCurrentNode(AMovementNode* Node) { CurrentNode = Node; }
	void SetIsMoving(bool bBool) { bIsMoving = bBool; }

	UFUNCTION(BlueprintCallable)
	bool GetIsMoving() const { return bIsMoving; }

	
	
	UFUNCTION(BlueprintCallable)
	void MoveForward();
	UFUNCTION(BlueprintCallable)
	void RotateRight();
	UFUNCTION(BlueprintCallable)
	void RotateLeft();
	UFUNCTION(BlueprintCallable)
	void RotateAround();
	
	void ResetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	
	
	
	void StartRotationTowardsDirection(const FVector& DirectionVector, float FallbackYawAmount);

	FQuat InitialVisualQuatRotation;
	FQuat TargetVisualQuatRotation;
	
	bool bIsVisualRotating = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Node System")
	AMovementNode* CurrentNode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Node System")
	AMovementNode* TargetNode;

	FVector TargetMoveLocation;
	FRotator TargetMoveRotation;
	bool bIsMoving = false;

	UPROPERTY()
	AMovementNode* NextForwardNode = nullptr;
	
	FRotator TargetForwardRotation;
	bool bShouldRotateToForwardNode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BaseMovementSpeed = 0.5;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float SlowAmount = 1;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float SpeedAmount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Node System")
	float NodeReachedThreshold; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Node System")
	float ForwardVectorMatchThreshold = 0.707f; 

	
	
	AMovementNode* FindNearestMovementNode() const;
	
	AMovementNode* FindBestNodeInDirection(const FVector& DirectionVector, float MinimumDotProduct) const;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
	
	bool bIsInterpolating = false;
	float ElapsedInterpTime = 0.0f;
	float TotalInterpDuration = .5f; 
	float EaseExponentRotation = 3.0f;
	float EaseExponentLocation = 1.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "GamePlay")
	int Score = 0;
};
