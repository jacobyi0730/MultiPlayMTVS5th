// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYMTVS5TH_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	// 내가 로컬플레이어라면 
	// 서버RPC를 통해 GI의 MySessionName을 내 유저이름으로 등록하고싶다.
	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetUserName(const FString& InUserName);
};
