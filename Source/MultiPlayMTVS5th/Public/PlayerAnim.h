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
	
};
