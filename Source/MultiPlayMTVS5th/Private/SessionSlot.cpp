// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionSlot.h"

#include "NetGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void USessionSlot::NativeConstruct()
{
	Super::NativeConstruct();
	Button_JoinSession->OnClicked.AddDynamic(this, &USessionSlot::OnMyJoinSession);
	GI = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());
}

void USessionSlot::OnMyJoinSession()
{
}

void USessionSlot::SetSessionInfo(const struct FSessionInfo& SessionInfo)
{
	SessionSearchIndex = SessionInfo.Index;
	Text_RoomName->SetText(FText::FromString(SessionInfo.RoomName));
	Text_HostName->SetText(FText::FromString(SessionInfo.HostName));
	FString playerCount = FString::Printf(TEXT("(%d/%d)"), SessionInfo.JoinPlayerCount, SessionInfo.MaxPlayer);
	Text_PlayerCount->SetText(FText::FromString(playerCount));
	FString ping = FString::Printf(TEXT("%dms"), SessionInfo.PingSpeed);
	Text_Ping->SetText(FText::FromString(ping));
}
