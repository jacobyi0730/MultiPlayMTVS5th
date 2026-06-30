// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYMTVS5TH_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TPS")
	bool bHasPistol = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TPS")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TPS")
	float Speed;
	
	UPROPERTY()
	TObjectPtr<class AMultiPlayMTVS5thCharacter> Player;
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category = "TPS")
	TObjectPtr<class UAnimMontage> FireMontage;
	
	void PlayFireMontage();
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "TPS")
	float PitchAngle; // -60 ~ 60
	
	
	UPROPERTY(EditAnywhere, Category = "TPS")
	TObjectPtr<class UAnimMontage> ReloadMontage;
	
	// 애니메이션 재생을 요청
	void PlayReloadMontage();
	
	// 애니메이션 재생 종료 함수(응답)
	UFUNCTION()
	void AnimNotify_ReloadFinished();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TPS")
	bool bDie;
	
};
