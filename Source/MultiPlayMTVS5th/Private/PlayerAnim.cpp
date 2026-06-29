// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "MultiPlayMTVS5thCharacter.h"

void UPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Player = Cast<AMultiPlayMTVS5thCharacter>(TryGetPawnOwner());
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Player)
	{
		FVector forward = Player->GetActorForwardVector();
		FVector right = Player->GetActorRightVector();
		FVector velocity = Player->GetVelocity();
		
		Direction = FVector::DotProduct(velocity, right); 
		Speed = FVector::DotProduct(velocity, forward); 
		bHasPistol = Player->bHasPistol;
		
		PitchAngle = -Player->GetBaseAimRotation().Pitch;
		PitchAngle = FMath::Clamp(PitchAngle, -60, 60);
	}
	
}

void UPlayerAnim::PlayFireMontage()
{
	if (!bHasPistol || !FireMontage)
		return;
	
	Montage_Play(FireMontage);
	
}
