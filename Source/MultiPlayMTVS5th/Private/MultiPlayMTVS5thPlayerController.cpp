// Copyright Epic Games, Inc. All Rights Reserved.


#include "MultiPlayMTVS5thPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "MainUI.h"
#include "Blueprint/UserWidget.h"
#include "MultiPlayMTVS5th.h"
#include "MultiPlayMTVS5thCharacter.h"
#include "MultiPlayMTVS5thGameMode.h"
#include "GameFramework/SpectatorPawn.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AMultiPlayMTVS5thPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		GM = Cast<AMultiPlayMTVS5thGameMode>(GetWorld()->GetAuthGameMode());
	}

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


void AMultiPlayMTVS5thPlayerController::ServerRPC_RespawnPlayer_Implementation()
{
	// Possess한 주인공을 제거하고
	auto player = GetPawn();
	if (player)
	{
		UnPossess();
		player->Destroy();
	}
	// 게임모드에게 재시작하라고 하고싶다.
	if (nullptr == GM)
	{
		GM = Cast<AMultiPlayMTVS5thGameMode>(GetWorld()->GetAuthGameMode());
	}
	GM->RestartPlayer(this);
}

void AMultiPlayMTVS5thPlayerController::ServerRPC_ChangeToSpectator_Implementation()
{
	// 구경꾼이 주인공위치에 생겨야한다.
	auto player = GetPawn();
	if (player)
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		auto* spectator = GetWorld()->SpawnActor<ASpectatorPawn>(GM->SpectatorClass,
			player->GetActorTransform(), params);
		
		Possess(spectator);
		// 구경꾼이 생성되면 주인공은 파괴
		player->Destroy();
		
		// 2초 후에 주인공 재시작
		FTimerHandle SpectatorTimerHandle;
		GetWorldTimerManager().SetTimer(
			SpectatorTimerHandle,
			this, 
			&AMultiPlayMTVS5thPlayerController::ServerRPC_RespawnPlayer_Implementation,
			2,
			false);
	}
}

// 서버에서 Pawn을 Possess한 이후에 클라이언트가 알림을 받는 시점
void AMultiPlayMTVS5thPlayerController::AcknowledgePossession(class APawn* player)
{
	Super::AcknowledgePossession(player);

	if (MainUIFactory && IsLocalController())
	{
		if (nullptr == MainUI)
		{
			MainUI = Cast<UMainUI>(CreateWidget(GetWorld(), MainUIFactory));
			MainUI->AddToViewport();
		}

		MyPlayer = Cast<AMultiPlayMTVS5thCharacter>(player);
		// 만약 구경꾼이라면 바로 종료
		if (nullptr == MyPlayer)
			return;
		
		if (MainUI)
		{
			MainUI->SetActiveCrosshair(false);

			MainUI->RemoveAllBullets();
			int32 max = MyPlayer->MaxBulletCount;
			for (int32 i = 0; i < max; i++)
			{
				MainUI->AddBullet();
			}
			MyPlayer->InitUI();
		}
	}


}
