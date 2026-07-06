// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlot.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYMTVS5TH_API USessionSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	TObjectPtr<class UNetGameInstance> GI;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_RoomName;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_HostName;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_PlayerCount;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Ping;
	
	int32 SessionSearchIndex;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> Button_JoinSession;
	
	UFUNCTION()
	void OnMyJoinSession();
	
	void SetSessionInfo(struct FSessionInfo& SessionInfo);
	
	
};
