// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYMTVS5TH_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	TObjectPtr<class UNetGameInstance> GI;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UEditableText>	Edit_UserName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UEditableText>	Edit_RoomName;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USlider> Slider_MaxPlayer;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_MaxPlayer;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_CreateRoom;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_FindRoom;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_GoFindRoom;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_GoMainRoom;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UScrollBox> Scroll_RoomList;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USessionSlot> SessionSlotFactroy;
	
	UFUNCTION()
	void AddSlot(const struct FSessionInfo& SessionInfo);
	
	UFUNCTION()
	void FindRoomButtonLock(bool bLock);
	
	
	UFUNCTION()
	void OnMyCreateRoom();
	
	UFUNCTION()
	void OnMySliderValueChanged(float value);
	
	UFUNCTION()
	void OnMyFindRoom();
	
	UFUNCTION()
	void OnMyGoMainRoom();

	UFUNCTION()
	void OnMyGoFindRoom();
};
