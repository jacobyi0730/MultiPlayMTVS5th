// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI.h"

#include "NetGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "SessionSlot.h"
#include "Components/ScrollBox.h"

void ULobbyUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	GI = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());
	
	GI->OnSearchSignatureComplete.AddDynamic(this, &ULobbyUI::AddSlot);
	
	Button_CreateRoom->OnClicked.AddDynamic(this, &ULobbyUI::OnMyCreateRoom);
	Button_FindRoom->OnClicked.AddDynamic(this, &ULobbyUI::OnMyFindRoom);
	Button_GoMainRoom->OnClicked.AddDynamic(this, &ULobbyUI::OnMyGoMainRoom);
	Button_GoFindRoom->OnClicked.AddDynamic(this, &ULobbyUI::OnMyGoFindRoom);;
	
	
	Slider_MaxPlayer->OnValueChanged.AddDynamic(this, &ULobbyUI::OnMySliderValueChanged);
	
	Text_MaxPlayer->SetText(FText::AsNumber(Slider_MaxPlayer->GetValue()));
	
	OnMyGoMainRoom();
}

void ULobbyUI::AddSlot(const struct FSessionInfo& SessionInfo)
{
	auto* slot = CreateWidget<USessionSlot>(this, SessionSlotFactroy);
	slot->SetSessionInfo(SessionInfo);
	Scroll_RoomList->AddChild(slot);
}

void ULobbyUI::OnMyCreateRoom()
{
	GI->OnMyCreateSession(
		Edit_RoomName->GetText().ToString(),
		Slider_MaxPlayer->GetValue());
}

void ULobbyUI::OnMySliderValueChanged(float value)
{
	Text_MaxPlayer->SetText(FText::AsNumber(value));
}

void ULobbyUI::OnMyFindRoom()
{
	GI->OnMyFindSessions();
}

void ULobbyUI::OnMyGoMainRoom()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void ULobbyUI::OnMyGoFindRoom()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}
