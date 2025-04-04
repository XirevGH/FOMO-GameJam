// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseInput_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

void UReverseInput_EffectComponent::ApplyEffect(AActor* Actor)
{
    Super::ApplyEffect(Actor);
    UE_LOG(LogTemp, Display, TEXT("ApplyEffect: ReverseInput"));
    
    // This is the player who triggered the power-up
    APlayerCharacter* TriggeringPlayer = Cast<APlayerCharacter>(Actor);
    if (TriggeringPlayer == nullptr)
    {
        return;
    }
    
    // Find the opponent player (the one who didn't trigger the power-up)
    APlayerCharacter* OpponentPlayer = nullptr;
    
    for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        APlayerCharacter* CurrentPlayer = *ActorItr;
        // Skip if null or if it's the triggering player
        if (CurrentPlayer && CurrentPlayer != TriggeringPlayer)
        {
            OpponentPlayer = CurrentPlayer;
            break; // Found the opponent, no need to continue loop
        }
    }
    
    if (OpponentPlayer == nullptr)
    {
        return;
    }
    
    APlayerController* OpponentController = Cast<APlayerController>(OpponentPlayer->GetController());
    if (OpponentController == nullptr)
    {
        return;
    }
    
    UInputComponent* InputComponent = OpponentPlayer->InputComponent;
    if (InputComponent == nullptr)
    {
        return;
    }
    
    // Reverse the opponent's inputs
    OpponentPlayer->ResetupPlayerInputComponent(InputComponent);
    
    // Set timer to restore opponent's normal inputs after duration
    FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([OpponentPlayer, InputComponent]()
    {
        if (OpponentPlayer)
        {
            OpponentPlayer->SetupPlayerInputComponent(InputComponent);
        }
    });
    
    GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
}