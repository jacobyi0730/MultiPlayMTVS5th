// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"

#include "EngineUtils.h"
#include "MultiPlayMTVS5thCharacter.h"


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
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();

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
