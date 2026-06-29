// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MultiPlayMTVS5thCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMultiPlayMTVS5thCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;


	
public:

	/** Constructor */
	AMultiPlayMTVS5thCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
public:
	void PrintNetLog();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pistol")
	TObjectPtr<class USceneComponent> GrabComp;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* IA_TakePistol;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* IA_Fire;

	void TakePistol(const FInputActionValue& InputActionValue);
	void MyFire(const FInputActionValue& InputActionValue);
	
	UPROPERTY()
	TArray<AActor*> Pistols;
	
	bool bHasPistol;
	
	UPROPERTY()
	TObjectPtr<AActor> OwnedPistol;
	
	float SearchDistanceToPistol = 200.f;


	void GrabPistol();
	void ReleasePistol();
	
	void AttachPistol(AActor* pistolActor);
	
	void DetachPistol(AActor* pistolActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pistol")
	TObjectPtr<class UNiagaraSystem> BulletImpactFactory; 
	
	UPROPERTY()
	TObjectPtr<class AMultiPlayMTVS5thPlayerController> PlayerController;
	
	
	// 탄창
	// 총알의 현재수, 최대수
	UPROPERTY(EditAnywhere)
	int32 MaxBulletCount = 10;
	int32 CurBulletCount = MaxBulletCount;
};

