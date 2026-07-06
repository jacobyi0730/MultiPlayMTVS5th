// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"

#include "OnlineSubsystem.h"

void UNetGameInstance::Init()
{
	Super::Init();
	
	if (auto* subsys = IOnlineSubsystem::Get())
	{
		SessionInterface = subsys->GetSessionInterface();
		if (SessionInterface)
		{
			
		}
	}
	
	
	
	
}

void UNetGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UNetGameInstance::OnMyCreateSession()
{
}

