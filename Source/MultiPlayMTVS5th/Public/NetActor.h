// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

UCLASS()
class MULTIPLAYMTVS5TH_API ANetActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USceneComponent> RootComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	void PrintNetLog();
	void FindOwner();
	
	UPROPERTY(EditAnywhere)
	float SearchDistance = 200.f;

	UPROPERTY(ReplicatedUsing=OnRep_RotYaw)
	float RotYaw;
	UFUNCTION()
	void OnRep_RotYaw();
	
	float CurTime;
	float LastTime;
	
	
	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> Mat;
	
	UPROPERTY(Replicated)
	int32 Loc;
	
	UPROPERTY(ReplicatedUsing=OnRep_MatColor)
	FLinearColor MatColor;
	UFUNCTION()
	void OnRep_MatColor();
	
	FTimerHandle TimerHandleMatColor;
	
	UPROPERTY(ReplicatedUsing=OnRep_NetNumber)
	int32 NetNumber;
	UFUNCTION()
	void OnRep_NetNumber();
	
	
	// ServerRPC, ClientRPC, NetMulticastRPC
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_ChangeColor(const FLinearColor& NewColor);
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_ChangeColor(const FLinearColor& NewColor);
	
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastRPC_ChangeColor(const FLinearColor& NewColor);

	
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};



