// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseInput_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

void UReverseInput_EffectComponent::ApplyEffect(AActor* Actor)
{
    Super::ApplyEffect(Actor);
    UE_LOG(LogTemp, Display, TEXT("ApplyEffect: ReverseInput"));
    
    APlayerCharacter* TriggeringPlayer = Cast<APlayerCharacter>(Actor);
    if (TriggeringPlayer == nullptr)
    {
        return;
    }
    
    APlayerCharacter* OpponentPlayer = nullptr;
    
    for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        APlayerCharacter* CurrentPlayer = *ActorItr;

        if (CurrentPlayer && CurrentPlayer != TriggeringPlayer)
        {
            OpponentPlayer = CurrentPlayer;
            break;
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
    
    OpponentPlayer->bHasInvertedInput = true;
    
    FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([OpponentPlayer]()
    {
        if (OpponentPlayer)
        {
            OpponentPlayer->bHasInvertedInput = false;
        }
    });
    
    GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
}