// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include <string>

void UNetGameInstance::Init()
{
	Super::Init();

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UNetGameInstance::OnNetworkFailure);
	}

	if (auto* subsys = IOnlineSubsystem::Get())
	{
		SessionInterface = subsys->GetSessionInterface();
		if (SessionInterface)
		{
			CreateSessionDelegateHandle = SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this, &UNetGameInstance::OnMyCreateSessionComplete);

			FindSessionDelegateHandle = SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
				this, &UNetGameInstance::OnMyFindSessionsComplete);

			JoinSessionDelegateHandle = SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
				this, &UNetGameInstance::OnMyJoinSessionComplete);

			DestroySessionDelegateHandle = SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
				this, &UNetGameInstance::OnMyDestroySessionComplete);

			UserInviteDelegateHandle = SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(
				this, &UNetGameInstance::OnMyInviteAcceptedComplete);
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
	SessionInterface->OnCreateSessionCompleteDelegates.Remove(CreateSessionDelegateHandle);
	SessionInterface->OnFindSessionsCompleteDelegates.Remove(FindSessionDelegateHandle);
	SessionInterface->OnJoinSessionCompleteDelegates.Remove(JoinSessionDelegateHandle);
	SessionInterface->OnDestroySessionCompleteDelegates.Remove(DestroySessionDelegateHandle);
	SessionInterface->OnSessionUserInviteAcceptedDelegates.Remove(UserInviteDelegateHandle);
	if (GEngine)
	{
		GEngine->OnNetworkFailure().RemoveAll(this);
	}
	
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
	settings.Set(FName("ROOM_NAME"), StringBase64Encoder(roomName),
	             EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	settings.Set(FName("HOST_NAME"), StringBase64Encoder(MySessionName),
	             EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().
	                                    GetUniqueNetId();

	UE_LOG(LogTemp, Warning, TEXT("OnMyCreateSession : %s"), *MySessionName);

	SessionInterface->CreateSession(*netID, FName(MySessionName), settings);
}

void UNetGameInstance::OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMyCreateSessionComplete : SessionName : %s, bWasSuccessful : %d"),
	       *SessionName.ToString(), bWasSuccessful);
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateSession Success!!!"));
		GetWorld()->ServerTravel(TEXT("/Game/MutiPlayer/Maps/BattleMap?listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateSession Failed..."));
	}
}

void UNetGameInstance::OnMyFindSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = FName("NULL") == IOnlineSubsystem::Get()->GetSubsystemName();
	SessionSearch->MaxSearchResults = 30;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	OnSearchLockComplete.Broadcast(true);
}

void UNetGameInstance::OnMyFindSessionsComplete(bool bWasSuccessful)
{
	OnSearchLockComplete.Broadcast(false);
	if (bWasSuccessful)
	{
		auto results = SessionSearch->SearchResults;

		for (int32 i = 0; i < results.Num(); i++)
		{
			auto& ssr = results[i];
			if (false == ssr.IsValid()) continue;

			FSessionInfo sessionInfo;

			sessionInfo.Index = i;

			ssr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.RoomName);
			ssr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.HostName);
			
			sessionInfo.RoomName = StringBase64Decoder(sessionInfo.RoomName);
			sessionInfo.HostName = StringBase64Decoder(sessionInfo.HostName);

			sessionInfo.MaxPlayer = ssr.Session.SessionSettings.NumPublicConnections;
			// 현재 입장 수 = 총수 - 입장가능수
			sessionInfo.JoinPlayerCount = sessionInfo.MaxPlayer - ssr.Session.NumOpenPublicConnections;
			sessionInfo.PingSpeed = ssr.PingInMs;

			sessionInfo.Print();

			if (OnSearchComplete.IsBound())
			{
				OnSearchComplete.Broadcast(sessionInfo);
			}
		}
	}
}

void UNetGameInstance::OnMyJoinSession(int32 index)
{
	auto sr = SessionSearch->SearchResults[index];
	SessionInterface->JoinSession(0, FName(MySessionName), sr);
}

void UNetGameInstance::OnMyJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (false == SessionInterface.IsValid())
		return;
	if (Result != EOnJoinSessionCompleteResult::Type::Success)
		return;

	FString url;
	SessionInterface->GetResolvedConnectString(SessionName, url);
	UE_LOG(LogTemp, Warning, TEXT("join url : %s"), *url);
	auto* pc = GetWorld()->GetFirstPlayerController();
	if (false == url.IsEmpty() && pc)
	{
		pc->ClientTravel(url, TRAVEL_Absolute);
	}
}

void UNetGameInstance::OnMyExitRoom()
{
	SessionInterface->DestroySession(FName(MySessionName));
}

void UNetGameInstance::OnMyDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		auto pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(TEXT("/Game/MutiPlayer/Maps/LobbyMap"), TRAVEL_Absolute);
	}
}

void UNetGameInstance::OnMyInviteAcceptedComplete(bool bWasSuccessful, int ControllerId,
                                                  TSharedPtr<const FUniqueNetId> UserId,
                                                  const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful)
	{
		FString roomName;
		InviteResult.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		roomName = StringBase64Decoder(roomName);
		SessionInterface->JoinSession(0, FName(roomName), InviteResult);
	}
}

void UNetGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type ErrorType,
                                        const FString& String)
{
	switch (ErrorType)
	{
	case ENetworkFailure::Type::ConnectionLost:
		OnMyExitRoom();
		break;
	}
}

FString UNetGameInstance::StringBase64Encoder(const FString& str)
{
	std::string utf8string = TCHAR_TO_UTF8(*str);
	TArray<uint8> bytes = TArray<uint8>((uint8*)utf8string.c_str(), utf8string.length());
	return FBase64::Encode(bytes);
}

FString UNetGameInstance::StringBase64Decoder(const FString& str)
{
	TArray<uint8> bytes;
	FBase64::Decode(str, bytes);
	std::string utf8string((char*)bytes.GetData(), bytes.Num());
	return UTF8_TO_TCHAR(utf8string.c_str());
}
