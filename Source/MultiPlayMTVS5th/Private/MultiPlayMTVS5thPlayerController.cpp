// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiPlayMTVS5thPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "MainUI.h"
#include "Blueprint/UserWidget.h"
#include "MultiPlayMTVS5th.h"
#include "MultiPlayMTVS5thCharacter.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AMultiPlayMTVS5thPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogMultiPlayMTVS5th, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
}

void AMultiPlayMTVS5thPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AMultiPlayMTVS5thPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AMultiPlayMTVS5thPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (MainUIFactory && IsLocalController())
	{
		MyPlayer = Cast<AMultiPlayMTVS5thCharacter>(GetCharacter());
		MainUI = Cast<UMainUI>(CreateWidget(GetWorld(), MainUIFactory));
		if (MainUI)
		{
			MainUI->AddToViewport();
			MainUI->SetActiveCrosshair(false);

			int32 max = MyPlayer->MaxBulletCount;
			for (int32 i = 0; i < max; i++)
			{
				MainUI->AddBullet();
			}
		}
	}
}
