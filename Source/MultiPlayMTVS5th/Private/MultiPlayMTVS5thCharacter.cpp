// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiPlayMTVS5thCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HPBarUI.h"
#include "InputActionValue.h"
#include "MainUI.h"
#include "MultiPlayMTVS5th.h"
#include "MultiPlayMTVS5thPlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerAnim.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"

AMultiPlayMTVS5thCharacter::AMultiPlayMTVS5thCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CameraBoom->TargetArmLength = 150.0f;
	CameraBoom->SocketOffset = FVector(0.f, 40.f, 60.f);

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	GrabComp = CreateDefaultSubobject<USceneComponent>(TEXT("GrabComp"));
	GrabComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	GrabComp->SetRelativeLocationAndRotation(
		FVector(-16.159421, 2.178094, 3.869521),
		FRotator(0.508895, 86.602734, 8.509347)
	);
	
	
	HpComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpComp"));
	HpComp->SetupAttachment(GetMesh());
}

void AMultiPlayMTVS5thCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 만약 나의 컨트롤러가 플레이 컨트롤러가 아니라면 InitUI를 호출하고싶다.
	if (nullptr == Cast<APlayerController>(GetController()))
	{
		InitUI();
	}

	// 세상에 있는 모든 총을 검색해서 Pistols에 넣고싶다.
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(),
	                                             AActor::StaticClass(),
	                                             TEXT("Pistol"),
	                                             Pistols);
}

void AMultiPlayMTVS5thCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PrintNetLog();
}

void AMultiPlayMTVS5thCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &AMultiPlayMTVS5thCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this,
		                                   &AMultiPlayMTVS5thCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &AMultiPlayMTVS5thCharacter::Look);


		EnhancedInputComponent->BindAction(IA_TakePistol, ETriggerEvent::Started, this,
		                                   &AMultiPlayMTVS5thCharacter::TakePistol);

		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &AMultiPlayMTVS5thCharacter::MyFire);
		
		EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Started, this, &AMultiPlayMTVS5thCharacter::ReloadPistol);
	}
	else
	{
		UE_LOG(LogMultiPlayMTVS5th, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AMultiPlayMTVS5thCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMultiPlayMTVS5thCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMultiPlayMTVS5thCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMultiPlayMTVS5thCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMultiPlayMTVS5thCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMultiPlayMTVS5thCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AMultiPlayMTVS5thCharacter::PrintNetLog()
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

void AMultiPlayMTVS5thCharacter::TakePistol(const FInputActionValue& InputActionValue)
{
	// 만약 리로드 중이라면 종료
	if (bReloadPistol)
		return;

	// 만약 총을 소지하고있으면 버려야한다.
	if (bHasPistol)
	{
		ReleasePistol();
	}
	else
	{
		GrabPistol();
	}
}

void AMultiPlayMTVS5thCharacter::MyFire(const FInputActionValue& InputActionValue)
{
	// 총을 들고 있지 않으면 바로 종료
	// 또는 현재 총알이 0이하라면 바로 종료
	// 또는 리로드 중이라면 바로 종료
	if (false == bHasPistol || 0 >= CurBulletCount || bReloadPistol)
		return;

	CurBulletCount--;
	PlayerController->MainUI->PopBullet(CurBulletCount);
	// 총쏘기(움찔)
	auto* anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (anim)
	{
		anim->PlayFireMontage();
	}

	// 총쏘기 구현..
	// 카메라 위치에서 카메라(FollowCamera) 앞방향으로 LineTrace, visibility
	FHitResult OutHit;
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 100000.f;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, params);

	if (bHit)
	{
		//OutHit.ImpactPoint
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BulletImpactFactory,
			OutHit.ImpactPoint,
			UKismetMathLibrary::MakeRotFromZ(OutHit.ImpactNormal)
		);
	}
}

void AMultiPlayMTVS5thCharacter::ReloadPistol(const FInputActionValue& InputActionValue)
{
	// 총을 잡고있지 않는다 혹은 리로드 중이라면 종료
	if (!bHasPistol || bReloadPistol)
		return;
	// 리로드 몽타주 재생.
	if (auto* anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance()))
	{
		anim->PlayReloadMontage();
	}
	bReloadPistol = true;
}

// 리로드 애니메이션 재생이 끝나면 호출해야함.
void AMultiPlayMTVS5thCharacter::OnReloadAmmo()
{
	CurBulletCount = MaxBulletCount;
	PlayerController->MainUI->RemoveAllBullets();
	
	for (int32 i=0 ; i<CurBulletCount ; i++)
	{
		PlayerController->MainUI->AddBullet();
	}
	bReloadPistol = false;
}

void AMultiPlayMTVS5thCharacter::GrabPistol()
{
	for (auto* pistol : Pistols)
	{
		if (pistol->GetOwner() != nullptr)
			continue;

		float dist = GetDistanceTo(pistol);
		if (dist > SearchDistanceToPistol)
			continue;

		OwnedPistol = pistol;
		OwnedPistol->SetOwner(this);
		bHasPistol = true;

		AttachPistol(OwnedPistol);
		break;
	}
}

void AMultiPlayMTVS5thCharacter::ReleasePistol()
{
	if (OwnedPistol)
	{
		bHasPistol = false;
		OwnedPistol->SetOwner(nullptr);
		DetachPistol(OwnedPistol);

		OwnedPistol = nullptr;
	}
}

void AMultiPlayMTVS5thCharacter::AttachPistol(AActor* pistolActor)
{
	auto* mesh = pistolActor->GetComponentByClass<UStaticMeshComponent>();
	mesh->SetSimulatePhysics(false);
	mesh->AttachToComponent(GrabComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	PlayerController->MainUI->SetActiveCrosshair(true);
}

void AMultiPlayMTVS5thCharacter::DetachPistol(AActor* pistolActor)
{
	auto* mesh = pistolActor->GetComponentByClass<UStaticMeshComponent>();
	mesh->SetSimulatePhysics(true);
	mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	PlayerController->MainUI->SetActiveCrosshair(false);
}

void AMultiPlayMTVS5thCharacter::InitUI()
{
	HPBarUI = Cast<UHPBarUI>(HpComp->GetWidget());

	PlayerController = Cast<AMultiPlayMTVS5thPlayerController>(GetController());
	
	// 체력을 모두 채우고싶다.
	// 나인가?
	if (PlayerController)
	{
		PlayerController->MainUI->UpdateHPBar(CurHP, MaxHP);
	}
	// 상대인가?
	else
	{
		HPBarUI->UpdateHPBar(CurHP, MaxHP);
	}
}
