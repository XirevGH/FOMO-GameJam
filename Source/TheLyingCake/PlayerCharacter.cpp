#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MovementNode.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    {
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
    }
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
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
    
    if (bIsVisualRotating)
    {
        FRotator CurrentRotation = GetActorRotation();
        
        ElapsedInterpTime += DeltaTime;

        float Alpha = FMath::Clamp(ElapsedInterpTime / TotalInterpDuration, 0.0f, 1.0f);
        Alpha = FMath::InterpEaseInOut(0.0f, 1.f, Alpha, EaseExponentRotation);
        
        FRotator InterpRotation = FMath::Lerp(CurrentRotation, TargetVisualRotation, Alpha);
        
        SetActorRotation(InterpRotation);
        if (Controller)
        {
            Controller->SetControlRotation(InterpRotation);
        }
        
        if (FMath::IsNearlyEqual(InterpRotation.Yaw, TargetVisualRotation.Yaw, .05f))
        {
            SetActorRotation(TargetVisualRotation);
            if (Controller)
            {
                Controller->SetControlRotation(TargetVisualRotation);
            }
            bIsVisualRotating = false;
            UE_LOG(LogTemp, Log, TEXT("Visual rotation complete. Yaw: %f"), TargetVisualRotation.Yaw);
        }
    }

    if (bIsMoving && TargetNode)
    {
        FVector LocationBeforeInterpolation = GetActorLocation();
        FRotator RotationBeforeInterpolation = GetActorRotation();

        ElapsedInterpTime += DeltaTime;

        float Alpha = FMath::Clamp(ElapsedInterpTime / TotalInterpDuration, 0.0f, 1.0f);
        Alpha = FMath::InterpEaseInOut(0.0f, 0.5f, Alpha, EaseExponentLocation);
        
        FVector InterpLocation = FMath::Lerp(LocationBeforeInterpolation, TargetMoveLocation, Alpha);
        FRotator InterpRotation = FMath::Lerp(RotationBeforeInterpolation, TargetMoveRotation, Alpha);
        
        SetActorLocationAndRotation(InterpLocation, InterpRotation);
        if (Controller)
        {
            Controller->SetControlRotation(InterpRotation);
        }
        
        FVector CurrentLocation = GetActorLocation();
        float DistSq = FVector::DistSquared(CurrentLocation, TargetMoveLocation);
        float ThresholdSq = FMath::Square(NodeReachedThreshold);
        
        UE_LOG(LogTemp, Log, TEXT("Tick Move: DeltaT=%.4f | CurrentLoc=%s | TargetLoc=%s | DistSq=%.3f | ThresholdSq=%.3f"),
               DeltaTime,
               *CurrentLocation.ToString(),
               *TargetMoveLocation.ToString(),
               DistSq,
               ThresholdSq);
        
        if (DistSq < ThresholdSq)
        {
            UE_LOG(LogTemp, Warning, TEXT(">>> Reached Node Threshold! Snapping. Node: %s"), *TargetNode->GetName());
            
            SetActorLocationAndRotation(TargetMoveLocation, TargetMoveRotation);
            if (Controller)
            {
                Controller->SetControlRotation(TargetMoveRotation);
            }
            
            CurrentNode = TargetNode;
            TargetNode = nullptr;
            bIsMoving = false;
            UE_LOG(LogTemp, Warning, TEXT("Tick Arrival: Setting bIsMoving = FALSE."));
        }
    }
    else if (bIsMoving && !TargetNode)
    {
        UE_LOG(LogTemp, Error, TEXT("Tick Move: bIsMoving=true but TargetNode is NULL! Stopping movement."));
        bIsMoving = false;
    }
}



void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForward);
        EnhancedInputComponent->BindAction(RotateRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateRight);
        EnhancedInputComponent->BindAction(RotateLeftAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateLeft);
        EnhancedInputComponent->BindAction(RotateAroundAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RotateAround);
    }
}

void APlayerCharacter::MoveForward(const FInputActionValue& Value)
{
    if (!Value.Get<bool>() || bIsMoving || bIsVisualRotating || !CurrentNode)
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
        TargetMoveRotation = (TargetMoveLocation - CurrentLocation).Rotation();

        ElapsedInterpTime = 0.0f;
        bIsMoving = true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Move Forward: No valid node found in front. (Dead End?)"));
        // TODO: Play a "bump" sound or visual feedback
    }
}

void APlayerCharacter::StartRotationTowards(float YawAmount)
{
    if (bIsMoving)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot rotate visually: Currently moving."));
        return;
    }

    FRotator CurrentVisualRotation = GetActorRotation();
    TargetVisualRotation = FRotator(CurrentVisualRotation.Pitch, FRotator::NormalizeAxis(CurrentVisualRotation.Yaw + YawAmount), CurrentVisualRotation.Roll);
    ElapsedInterpTime = 0.0f;
    bIsVisualRotating = true;
    UE_LOG(LogTemp, Log, TEXT("Starting visual rotation to Yaw: %f"), TargetVisualRotation.Yaw);
}

void APlayerCharacter::RotateRight(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && !bIsMoving && !bIsVisualRotating)
    {
        StartRotationTowards(90.0f);
    }
}

void APlayerCharacter::RotateLeft(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && !bIsMoving && !bIsVisualRotating)
    {
        StartRotationTowards(-90.0f);
    }
}

void APlayerCharacter::RotateAround(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && !bIsMoving && !bIsVisualRotating)
    {
        StartRotationTowards(180.0f);
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