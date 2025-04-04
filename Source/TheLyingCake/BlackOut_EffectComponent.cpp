// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackOut_EffectComponent.h"
#include "PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"


void UBlackOut_EffectComponent::ApplyEffect(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("EffectDuration: %f"), EffectDuration);
	Super::ApplyEffect(Actor);
	if (Actor != nullptr)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
		if (PlayerCharacter != nullptr)
		{
			
			UCameraComponent* PlayerCamera = PlayerCharacter->FindComponentByClass<UCameraComponent>();
			if (PlayerCamera)
			{
				PlayerCamera->PostProcessSettings.bOverride_ColorGamma = true;
				PlayerCamera->PostProcessSettings.ColorGamma = FVector4(0,0,0,0);
			}
			FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, PlayerCamera]()
			{
				PlayerCamera->PostProcessSettings.bOverride_ColorGamma = false;
				
				this->DestroyComponent();
			});
			
			GetOwner()->GetWorld()->GetTimerManager().SetTimer(EffectTimer, TimerDelegate, EffectDuration, false);
		}
	}
}
