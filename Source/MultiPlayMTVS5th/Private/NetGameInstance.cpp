// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

void UNetGameInstance::Init()
{
	Super::Init();
	
	if (auto* subsys = IOnlineSubsystem::Get())
	{
		SessionInterface = subsys->GetSessionInterface();
		if (SessionInterface)
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnMyCreateSessionComplete);
			
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnMyFindSessionsComplete);
		}
	}
	
	
	// FTimerHandle TimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	// {
	// 	OnMyCreateSession(MySessionName, 10);
	// }, 2, false);
	
	
}

void UNetGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UNetGameInstance::OnMyCreateSession(FString roomName, int32 maxPlayer)
{
	FOnlineSessionSettings settings;
	
	settings.bIsDedicated = false;
	// true 랜매치인가? false 스팀인가?
	settings.bIsLANMatch = FName("NULL") == IOnlineSubsystem::Get()->GetSubsystemName();
	// 매칭이 온라인에 노출시킬것인가?
	settings.bShouldAdvertise = true;
	// 온라인 상태 정보를 활용할것인가?
	settings.bUsesPresence = true;
	// 로비를 사용할것인가?
	settings.bUseLobbiesIfAvailable = true;
	// 게임진행중에 참가여부
	settings.bAllowJoinInProgress = true;
	settings.bAllowJoinViaPresence = true;
	
	settings.NumPublicConnections = maxPlayer;
	
	// 커스텀 설정
	settings.Set(FName("ROOM_NAME"), roomName, 
		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	settings.Set(FName("HOST_NAME"), MySessionName, 
		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	
	UE_LOG(LogTemp, Warning, TEXT("OnMyCreateSession : %s"), *MySessionName);
	
	SessionInterface->CreateSession(*netID, FName(MySessionName), settings);
}

void UNetGameInstance::OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMyCreateSessionComplete : SessionName : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);
}

void UNetGameInstance::OnMyFindSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = FName("NULL") == IOnlineSubsystem::Get() ->GetSubsystemName();
	SessionSearch->MaxSearchResults = 30;
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UNetGameInstance::OnMyFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		auto results = SessionSearch->SearchResults;
		
		for (auto& ssr : results)
		{
			if (false == ssr.IsValid()) continue;
			
			FSessionInfo sessionInfo;
			
			ssr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.RoomName);
			ssr.Session.SessionSettings.Get(FName("HOST_NAME"),  sessionInfo.HostName);
			
			sessionInfo.MaxPlayer = ssr.Session.SessionSettings.NumPublicConnections;
			// 현재 입장 수 = 총수 - 입장가능수
			sessionInfo.JoinPlayerCount = sessionInfo.MaxPlayer - ssr.Session.NumOpenPublicConnections;
			sessionInfo.PingSpeed = ssr.PingInMs;
			
			sessionInfo.Print();
		}
	}
}

