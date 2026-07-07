// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"

#include "NetGameInstance.h"

void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();
	auto pc = GetPlayerController();
	if (pc && pc->IsLocalController())
	{
		auto GI = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());
		if (GI)
		{
			ServerRPC_SetUserName(GI->MySessionName);
		}
	}
}

void ANetPlayerState::ServerRPC_SetUserName_Implementation(const FString& InUserName)
{
	SetPlayerName(InUserName);
}
