#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MovementNode.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
    
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
    
    MovementSpeed = 8.0f;
    RotationSpeed = 8.0f;
    NodeReachedThreshold = .05f;
    ForwardVectorMatchThreshold = 0.707f;

    CurrentNode = nullptr;
    TargetNode = nullptr;
    bIsMoving = false;
    bIsVisualRotating = false;

    UE_LOG(LogTemp, Warning, TEXT("Character Constructor Called"));
    
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("Character BeginPlay Called"));
   MovementSpeed = BaseMovementSpeed;
    CurrentNode = FindNearestMovementNode();
    if (CurrentNode)
    {
        FVector StartLocation = CurrentNode->GetActorLocation();
        FRotator StartRotation = GetActorRotation(); 
        if (CurrentNode->ConnectedNodes.Num() > 0 && CurrentNode->ConnectedNodes[0] != nullptr)
        {
            FVector DirectionToFirstNode = (CurrentNode->ConnectedNodes[0]->GetActorLocation() - StartLocation);
            if (!DirectionToFirstNode.IsNearlyZero())
            {
                StartRotation = DirectionToFirstNode.Rotation();
            }
        }

        SetActorLocationAndRotation(StartLocation, StartRotation);
        if (Controller)
        {
            Controller->SetControlRotation(StartRotation);
        }
        UE_LOG(LogTemp, Log, TEXT("Player started at Node: %s"), *CurrentNode->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Player could not find a starting Maze Node!"));
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MovementSpeed = BaseMovementSpeed * SlowAmount;
    if (bIsVisualRotating)
    {
        ElapsedInterpTime += DeltaTime;
        float Alpha = FMath::Clamp(ElapsedInterpTime / RotationSpeed, 0.0f, 1.0f);
        Alpha = FMath::InterpEaseInOut(0.0f, 1.f, Alpha, EaseExponentRotation);
        
        FQuat InterpQuat = FQuat::Slerp(InitialVisualQuatRotation, TargetVisualQuatRotation, Alpha);
        
        FRotator InterpRotation = InterpQuat.Rotator();
        SetActorRotation(InterpQuat);
        if (Controller)
        {
            Controller->SetControlRotation(InterpRotation);
        }
        
        if (Alpha >= 1.0f || InterpQuat.Equals(TargetVisualQuatRotation, 0.001f))
        {
            bIsVisualRotating = false;
            SetActorRotation(TargetVisualQuatRotation);
            if (Controller)
            {
                Controller->SetControlRotation(TargetVisualQuatRotation.Rotator());
            }
            UE_LOG(LogTemp, Log, TEXT("Visual rotation complete. Final Rotation: %s"), *TargetVisualQuatRotation.Rotator().ToString());
        }
    }

     if (bIsMoving && TargetNode)
    {
        FVector LocationBeforeInterpolation = GetActorLocation();
        FRotator RotationBeforeInterpolation = GetActorRotation();

        ElapsedInterpTime += DeltaTime;
        float Alpha = FMath::Clamp(ElapsedInterpTime / MovementSpeed, 0.0f, 1.0f);
        float LocationAlpha = FMath::InterpEaseInOut(0.0f, 0.5f, Alpha, EaseExponentLocation);
         
        FVector InterpLocation = FMath::Lerp(LocationBeforeInterpolation, TargetMoveLocation, LocationAlpha);
         
        FRotator InterpRotation;
        
        if (bShouldRotateToForwardNode && NextForwardNode)
        {
            float RotationBlendFactor = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, 2.0f);
            InterpRotation = FMath::Lerp(RotationBeforeInterpolation, TargetForwardRotation, RotationBlendFactor);
        }
        else
        {
            InterpRotation = FMath::Lerp(RotationBeforeInterpolation, TargetMoveRotation, Alpha);
        }
        
        SetActorLocationAndRotation(InterpLocation, InterpRotation);
        if (Controller)
        {
            Controller->SetControlRotation(InterpRotation);
        }
        
        FVector CurrentLocation = GetActorLocation();
        float DistSq = FVector::DistSquared(CurrentLocation, TargetMoveLocation);
        float ThresholdSq = FMath::Square(NodeReachedThreshold);
        
        if (DistSq < ThresholdSq)
        {
            FRotator FinalRotation = bShouldRotateToForwardNode ? TargetForwardRotation : TargetMoveRotation;
            
            SetActorLocationAndRotation(TargetMoveLocation, FinalRotation);
            if (Controller)
            {
                Controller->SetControlRotation(FinalRotation);
            }
            
            CurrentNode = TargetNode;
            TargetNode = nullptr;
            NextForwardNode = nullptr;
            bShouldRotateToForwardNode = false;
            bIsMoving = false;
        }
    }
    else if (bIsMoving && !TargetNode)
    {
        bIsMoving = false;
        bShouldRotateToForwardNode = false;
        NextForwardNode = nullptr;
    }
}



void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->ClearActionBindings();
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForward);
        EnhancedInputComponent->BindAction(RotateRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateRight);
        EnhancedInputComponent->BindAction(RotateLeftAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateLeft);
        EnhancedInputComponent->BindAction(RotateAroundAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateAround);
    }
}


void APlayerCharacter::ResetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
       
        EnhancedInputComponent->ClearActionBindings();
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateAround);
        EnhancedInputComponent->BindAction(RotateRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateLeft);
        EnhancedInputComponent->BindAction(RotateLeftAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateRight);
        EnhancedInputComponent->BindAction(RotateAroundAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForward);
    }
}

void APlayerCharacter::MoveForward()
{
    if (bIsMoving || bIsVisualRotating || !CurrentNode)
    {
        return;
    }
    
    AMovementNode* NextNode = nullptr;
    float BestDot = -1.1f;

    FVector CurrentLocation = GetActorLocation();
    FVector PlayerForward = GetActorForwardVector();

    UE_LOG(LogTemp, Log, TEXT("Attempting Move Forward from Node %s. Player Facing: %s"), *CurrentNode->GetName(), *PlayerForward.ToString());

    for (AMovementNode* ConnectedNode : CurrentNode->ConnectedNodes)
    {
        if (!ConnectedNode) continue;

        FVector DirectionToNode = (ConnectedNode->GetActorLocation() - CurrentLocation);
        if(DirectionToNode.IsNearlyZero()) continue;

        DirectionToNode.Normalize();
        float DotProduct = FVector::DotProduct(PlayerForward, DirectionToNode);

        UE_LOG(LogTemp, Verbose, TEXT("  Checking node %s. Direction: %s, Dot: %f"), *ConnectedNode->GetName(), *DirectionToNode.ToString(), DotProduct);
        
        if (DotProduct > BestDot && DotProduct >= ForwardVectorMatchThreshold)
        {
            BestDot = DotProduct;
            NextNode = ConnectedNode;
        }
    }
    
    if (NextNode)
    {
        UE_LOG(LogTemp, Log, TEXT("Found forward node: %s (Dot: %f). Starting Movement."), *NextNode->GetName(), BestDot);

        TargetNode = NextNode;
        TargetMoveLocation = TargetNode->GetActorLocation();
        
        FVector DirectionToTarget = TargetMoveLocation - CurrentLocation;
        TargetMoveRotation = DirectionToTarget.Rotation();
        
        NextForwardNode = nullptr;
        bShouldRotateToForwardNode = false;
        
        for (AMovementNode* PotentialForwardNode : TargetNode->ConnectedNodes)
        {
            if (!PotentialForwardNode || PotentialForwardNode == CurrentNode) continue;
            
            FVector NextNodeDirection = (PotentialForwardNode->GetActorLocation() - TargetNode->GetActorLocation()).GetSafeNormal();
            FVector MovementDirection = DirectionToTarget.GetSafeNormal();
            
            float ForwardDot = FVector::DotProduct(MovementDirection, NextNodeDirection);
            
            if (ForwardDot > ForwardVectorMatchThreshold)
            {
                NextForwardNode = PotentialForwardNode;
                
                FVector DirectionToForwardNode = NextForwardNode->GetActorLocation() - TargetNode->GetActorLocation();
                TargetForwardRotation = DirectionToForwardNode.Rotation();
                
                bShouldRotateToForwardNode = true;
                
                UE_LOG(LogTemp, Log, TEXT("Found next forward node: %s (Dot: %f)"), *NextForwardNode->GetName(), ForwardDot);
                break;
            }
        }

        ElapsedInterpTime = 0.0f;
        bIsMoving = true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Move Forward: No valid node found in front. (Dead End?)"));
        UGameplayStatics::SpawnSoundAttached(HitWallSOund, Mesh1P);
    }
}

AMovementNode* APlayerCharacter::FindBestNodeInDirection(const FVector& DirectionVector, float MinimumDotProduct) const
{
    if (!CurrentNode) return nullptr;
    
    AMovementNode* BestNode = nullptr;
    float BestDot = MinimumDotProduct;
    
    FVector CurrentLocation = GetActorLocation();
    
    for (AMovementNode* ConnectedNode : CurrentNode->ConnectedNodes)
    {
        if (!ConnectedNode) continue;
        
        FVector DirectionToNode = (ConnectedNode->GetActorLocation() - CurrentLocation);
        if (DirectionToNode.IsNearlyZero()) continue;
        
        DirectionToNode.Normalize();
        float DotProduct = FVector::DotProduct(DirectionVector, DirectionToNode);
        
        UE_LOG(LogTemp, Verbose, TEXT("  Direction check for node %s. Direction: %s, Dot with requested: %f"), 
               *ConnectedNode->GetName(), *DirectionToNode.ToString(), DotProduct);
        
        if (DotProduct > BestDot)
        {
            BestDot = DotProduct;
            BestNode = ConnectedNode;
        }
    }
    
    return BestNode;
}

void APlayerCharacter::StartRotationTowardsDirection(const FVector& DirectionVector, float FallbackYawAmount)
{
    if (bIsMoving || bIsVisualRotating)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot rotate: Currently moving or already rotating."));
        return;
    }
    
    InitialVisualQuatRotation = GetActorQuat();
    
    AMovementNode* NodeInDirection = FindBestNodeInDirection(DirectionVector, 0.5f);
    
    if (NodeInDirection)
    {
        FVector DirectionToNode = NodeInDirection->GetActorLocation() - GetActorLocation();
        FRotator TargetRotation = DirectionToNode.Rotation();
        
        TargetVisualQuatRotation = TargetRotation.Quaternion();
        
        UE_LOG(LogTemp, Log, TEXT("Rotating towards node: %s at angle: %s"), 
               *NodeInDirection->GetName(), *TargetRotation.ToString());
    }
    else
    {
        FQuat DeltaQuat = FQuat(FRotator(0.0f, FallbackYawAmount, 0.0f));
        TargetVisualQuatRotation = InitialVisualQuatRotation * DeltaQuat;
        
        UE_LOG(LogTemp, Log, TEXT("No node found in direction, using fallback rotation of %f degrees"), FallbackYawAmount);
    }
    
    TargetVisualQuatRotation.Normalize();
    ElapsedInterpTime = 0.0f;
    bIsVisualRotating = true;

    UGameplayStatics::SpawnSoundAttached(TurnSwooshSOund, Mesh1P);
}

void APlayerCharacter::RotateRight()
{
    if (!bIsMoving && !bIsVisualRotating)
    {
        FVector RightDirection = GetActorRightVector();
        StartRotationTowardsDirection(RightDirection, 90.0f);
    }
}

void APlayerCharacter::RotateLeft()
{
    if (!bIsMoving && !bIsVisualRotating)
    {
        FVector LeftDirection = -GetActorRightVector();
        StartRotationTowardsDirection(LeftDirection, -90.0f);
    }
}

void APlayerCharacter::RotateAround()
{
    if (!bIsMoving && !bIsVisualRotating)
    {
        FVector BackDirection = -GetActorForwardVector();
        StartRotationTowardsDirection(BackDirection, 180.0f);
    }
}

AMovementNode* APlayerCharacter::FindNearestMovementNode() const
{
    TArray<AActor*> FoundNodes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMovementNode::StaticClass(), FoundNodes);

    AMovementNode* NearestNode = nullptr;
    float MinDistSq = TNumericLimits<float>::Max();
    FVector PlayerLocation = GetActorLocation();

    for (AActor* NodeActor : FoundNodes)
    {
        float DistSq = FVector::DistSquared(PlayerLocation, NodeActor->GetActorLocation());
        if (DistSq < MinDistSq)
        {
            MinDistSq = DistSq;
            NearestNode = Cast<AMovementNode>(NodeActor);
        }
    }
    return NearestNode;
}