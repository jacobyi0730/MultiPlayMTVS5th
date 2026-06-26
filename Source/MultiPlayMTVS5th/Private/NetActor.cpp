// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"

#include "EngineUtils.h"
#include "MultiPlayMTVS5thCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


// Sets default values
ANetActor::ANetActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();
	
	Mat = MeshComp->CreateDynamicMaterialInstance(0);
	
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(TimerHandleMatColor, [&]()
		{
			//MatColor = FLinearColor::MakeRandomColor();
			//OnRep_MatColor();

			ServerRPC_ChangeColor(FLinearColor::MakeRandomColor());
			
			NetNumber++;
			MARK_PROPERTY_DIRTY_FROM_NAME(ANetActor, NetNumber, this);
			
		}, 1, true);
	}
}

void ANetActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(TimerHandleMatColor);
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();
	FindOwner();
	
	// 서버에서 회전하고싶다.
	if (HasAuthority())
	{
		AddActorLocalRotation(FRotator(0, 50 * DeltaTime, 0));
		RotYaw = GetActorRotation().Yaw;
	}
	else
	{
		CurTime += DeltaTime;
		// LastTime가 0이면 0으로 나눠야하는 상황이 발생하니까. 예외처리 한다.
		if (LastTime < KINDA_SMALL_NUMBER)
		{
			return;
		}
		
		// 예측
		float newYaw = RotYaw + 50 * LastTime;
		
		// 보간
		float lerpRatio = CurTime / LastTime;
		float lerpYaw = FMath::Lerp(RotYaw, newYaw, lerpRatio);
		FRotator rot = GetActorRotation();
		rot.Yaw = lerpYaw;
		SetActorRotation(rot);
	}
	
	FString str = FString::Printf(TEXT("%d"), NetNumber);
	FVector loc = GetActorLocation() + FVector(0, 0, -50.f);
	DrawDebugString(GetWorld(), loc, str, nullptr, FColor::Yellow, 0, true, 2);

}

//
// void Test()
// {
// 	int32 length = 1;
// 	
// 	TArray<uint8> buf;
// 	
// 	length >> (24)
// 	
// 	length >> (16) & 0xff
// 	
// 	length >> (8) & 0xff
// 	
// 	length & 0xff
// 	
// 	uint8 value = 1;
// 	value = value >> 1;
// 	
// 	
// 	uint8 upper = 1; // 0001
// 	uint8 side = 2;	// 0010
// 	uint8 lower = 4;// 0100
// 	
// 	uint8 f = upper | lower; // 0101
// 	
// 	if ( f & side != 0)
// 		0101
// 	    0010
// 	
// }

void ANetActor::OnRep_RotYaw()
{
	// 클라이언트
	FRotator rot = GetActorRotation();
	rot.Yaw = RotYaw;
	SetActorRotation(rot);
	// 현재 시간을 저장
	LastTime = CurTime;
	// 현재시간을 초기화
	CurTime = 0;
}

void ANetActor::PrintNetLog()
{
	// 커넥션
	FString strConn = nullptr == GetNetConnection() ? TEXT("Invalid Connection") : TEXT("Valid Connection");
	// 오너
	FString strOwner = nullptr == GetOwner() ? TEXT("No Owner") : GetOwner()->GetName();


	FString lRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString rRole = UEnum::GetValueAsString<ENetRole>(GetRemoteRole());

	FString netLog = FString::Printf(
		TEXT("Conn : %s\nOwner : %s\nLocalRole : %s\nRemoteRole : %s"), *strConn, *strOwner, *lRole, *rRole);

	DrawDebugString(GetWorld(), GetActorLocation(), netLog, nullptr, FColor::Yellow, 0, true, 1);
}

void ANetActor::FindOwner()
{
	// 권한이 있어야한다(서버에서 해야한다)
	if (HasAuthority())
	{
		// 반경 SearchDistance안의 가장 가까운 AMultiPlayMTVS5thCharacter를 오너로 하고싶다.
		// 반복하면서 거리를 측정한다. 
		// 그 거리를 기억하고있어야한다.
		AActor* newOwner = nullptr;
		float dist = 99999999.f;
		for (TActorIterator<AActor> It(GetWorld(), AMultiPlayMTVS5thCharacter::StaticClass()); It; ++It)
		{
			AActor* Actor = *It;
			float tempDist = this->GetDistanceTo(Actor);
			
			if (SearchDistance < tempDist)
				continue;
			
			if (nullptr == newOwner || tempDist < dist)
			{
				newOwner = Actor;
				dist = tempDist;
			}
		}
		if (GetOwner() != newOwner)
		{
			SetOwner(newOwner);
		}
	}
}


void ANetActor::OnRep_MatColor()
{
	if (Mat)
	{
		Mat->SetVectorParameterValue(TEXT("FloorColor"), MatColor);
	}
}

void ANetActor::OnRep_NetNumber()
{
	bAlwaysRelevant = true;
}

bool ANetActor::ServerRPC_ChangeColor_Validate(const FLinearColor& NewColor)
{
	return true;
}

void ANetActor::ServerRPC_ChangeColor_Implementation(const FLinearColor& NewColor)
{
	// 서버 응답
	if (HasAuthority())
	{
		NetMulticastRPC_ChangeColor(NewColor);
		// ClientRPC_ChangeColor_Implementation(NewColor);
		// ClientRPC_ChangeColor(NewColor);
	}
}

void ANetActor::ClientRPC_ChangeColor_Implementation(const FLinearColor& NewColor)
{
	// 클라이언트 응답
	//if (!HasAuthority())
	{
		if (Mat)
		{
			Mat->SetVectorParameterValue(TEXT("FloorColor"), NewColor);
		}
	}
}

void ANetActor::NetMulticastRPC_ChangeColor_Implementation(const FLinearColor& NewColor)
{
	// 클라이언트 응답
	//if (!HasAuthority())
	{
		if (Mat)
		{
			Mat->SetVectorParameterValue(TEXT("FloorColor"), NewColor);
		}
	}
}

bool ANetActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if (Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation))
	{
		return true;
	}
	// 
	
	return false;
}

void ANetActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams params;
	DOREPLIFETIME_WITH_PARAMS_FAST(ANetActor, RotYaw, params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ANetActor, MatColor, params);
	
	DOREPLIFETIME(ANetActor, Loc);
	
	FDoRepLifetimeParams params2;
	params2.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ANetActor, NetNumber, params2);
}
