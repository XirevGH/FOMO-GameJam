// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseInput_EffectComponent.h"
#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"  // Required for handling input actions
#include "InputMappingContext.h"

void UReverseInput_EffectComponent::ApplyEffect(AActor* Actor)
{
	Super::ApplyEffect(Actor);

	EffectDuration = 2;
	
	Super::ApplyEffect(Actor);
	UE_LOG(LogTemp, Display, TEXT("ApplyEffect: Slow"));
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			//reverse the input
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
