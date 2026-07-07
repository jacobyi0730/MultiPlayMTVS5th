// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString RoomName;
	
	UPROPERTY(BlueprintReadWrite)
	FString HostName;
	
	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayer;
	
	UPROPERTY(BlueprintReadWrite)
	int32 JoinPlayerCount;

	UPROPERTY(BlueprintReadWrite)
	int32 PingSpeed;
	
	UPROPERTY(BlueprintReadWrite)
	int32 Index;
	
	void Print()
	{
		FString log = FString::Printf(TEXT("[%d]%s : %s, %d/%d %dms"), 
			Index,
			*RoomName,
			*HostName,
			JoinPlayerCount,
			MaxPlayer,
			PingSpeed);
		
		UE_LOG(LogTemp, Warning, TEXT("%s"), *log);
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const struct FSessionInfo&, SessionInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchLockSignature, bool, bSearching);

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
	
	FSearchSignature OnSearchComplete;
	FSearchLockSignature OnSearchLockComplete;
	
	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle FindSessionDelegateHandle;
	FDelegateHandle JoinSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;
	FDelegateHandle UserInviteDelegateHandle;
	

	virtual void Init() override;
	
	virtual void Shutdown() override;
	
	// 방생성 요청
	void OnMyCreateSession(FString roomName, int32 maxPlayer);
	// 방생성 응답
	void OnMyCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	
	// 방검색
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	void OnMyFindSessions();
	void OnMyFindSessionsComplete(bool bWasSuccessful);
	
	// 방참여 요청
	void OnMyJoinSession(int32 index);
	// 방생성 응답
	void OnMyJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	// 방퇴장 요청
	void OnMyExitRoom();
	// 방퇴장 응답
	void OnMyDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void OnMyInviteAcceptedComplete(bool bWasSuccessful, int ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type ErrorType, const FString& String);
	
	
	FString StringBase64Encoder(const FString& str);
	FString StringBase64Decoder(const FString& str);

};
