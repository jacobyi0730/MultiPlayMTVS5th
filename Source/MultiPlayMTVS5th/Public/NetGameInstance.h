// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYMTVS5TH_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	IOnlineSessionPtr SessionInterface;
	FString MySessionName = TEXT("Jacobyi");
	
	virtual void Init() override;
	
	virtual void Shutdown() override;
	
	// 방생성 요청
	void OnMyCreateSession(FString roomName, int32 maxPlayer);
	// 방생성 응답
	UFUNCTION()
	void OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	
};
