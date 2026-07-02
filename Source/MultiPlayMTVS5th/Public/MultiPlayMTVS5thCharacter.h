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

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* IA_Reload;
	
	void TakePistol(const FInputActionValue& InputActionValue);
	void MyFire(const FInputActionValue& InputActionValue);
	void ReloadPistol(const FInputActionValue& InputActionValue);
	
	void OnReloadAmmo();
	
	UPROPERTY()
	TArray<AActor*> Pistols;
	
	UPROPERTY(Replicated)
	bool bHasPistol;	
	bool bReloadPistol;
	
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
	UPROPERTY(Replicated)
	int32 CurBulletCount = MaxBulletCount;

	// 체력
	UPROPERTY(EditAnywhere)
	int32 MaxHP = 3;

	UPROPERTY(ReplicatedUsing=OnRep_CurHP)
	int32 CurHP = 3;
	UFUNCTION()
	void OnRep_CurHP();
	
	
	
	// 프로퍼티
	__declspec(property(put=SetHP, get=GetHP)) int32 HP;
	void SetHP(int32 newHP);
	int32 GetHP();
	
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	TObjectPtr<class UWidgetComponent> HpComp;
	
	UPROPERTY()
	TObjectPtr<class UHPBarUI> HPBarUI;
	
	void InitUI();
	
	// 피격처리
	void DamageProcess(int32 damage);
	
	UPROPERTY() // 리플리케이트해야함.
	bool bDie;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_GrabPistol();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMultiRPC_GrabPistol(AActor* pistolActor);
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_ReleasePistol();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMultiRPC_ReleasePistol(AActor* pistolActor);
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire();
	
	UFUNCTION(NetMulticast, Unreliable)
	void NetMultiRPC_Fire(bool bHit, const FHitResult& OutHit);
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_DamageAnim();
	
	UPROPERTY(EditDefaultsOnly, Category="Pistol")
	TSubclassOf<class UCameraShakeBase> DamageCameraShake;
	
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_Reload();
	
	UFUNCTION(NetMulticast, Unreliable)
	void NetMultiRPC_Reload();
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_Reload(int32 bulletCount);
	
	
	
	
	
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};

