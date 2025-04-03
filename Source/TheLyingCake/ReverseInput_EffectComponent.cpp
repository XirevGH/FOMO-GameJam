// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseInput_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

void UReverseInput_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);

	EffectDuration = 10;
	
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Display, TEXT("ApplyEffect: Slow"));
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			//reverse the input
			for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				APlayerCharacter* OtherPlayer = *ActorItr;
				if (OtherPlayer != nullptr && OtherPlayer != PlayerCharacter)
				{

					PlayerCharacter = Cast<APlayerCharacter>(OtherPlayer);

					APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
			
					if (PlayerController != nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("PlayerController work"));
						APawn* ControlledPawn = PlayerController->GetPawn();
						UInputComponent* InputComponent = ControlledPawn->InputComponent;
				
						if (InputComponent  != nullptr)
						{
							UE_LOG(LogTemp, Warning, TEXT("InputComponent  work"));

					
							PlayerCharacter->ResetupPlayerInputComponent(InputComponent );
			
							FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, PlayerCharacter, InputComponent ]()
							{
				
								PlayerCharacter->SetupPlayerInputComponent(InputComponent );
			
								this->DestroyComponent();
							});
							GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
						}
					}
				}
			}
			
		}
	}
	
}
