// Copyright Epic Games, Inc. All Rights Reserved.

#include "Proje1Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.h"
#include "AIChar.h" // Düþman karakter
#include "AI4GOZ.h" // Düþman karakter
#include "Components/BoxComponent.h"


//////////////////////////////////////////////////////////////////////////
// AProje1Character

AProje1Character::AProje1Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	PrimaryActorTick.bCanEverTick = true;

	// Kýlýç çarpýþma kutusu
	SwordHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordHitBox"));
	SwordHitBox->SetupAttachment(GetMesh(), TEXT("WeaponSocket")); // Skeleton mesh'te SwordSocket baðlanacak
	SwordHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwordHitBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SwordHitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwordHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PrimaryActorTick.bCanEverTick = true;
	Health11 = 100.f; // Ana karakterin baþlangýç caný

}

void AProje1Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	SwordHitBox->OnComponentBeginOverlap.AddDynamic(this, &AProje1Character::OnSwordOverlap);

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	CurrentHealth = MaxHealth;

	// SADECE Desert/Forest HARÝTASINDA CAN BARINI GÖSTER
	FString CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // Örn: UEDPIE_0_ gibi prefixleri temizlemeye yarýyo

	if (CurrentMapName == "Desert" || CurrentMapName == "Forest" || CurrentMapName == "TestMap")
	{
		if (HealthBarWidgetClass)
		{
			HealthBarWidgetInstance = Cast<UHealthBarWidget>(CreateWidget<UHealthBarWidget>(GetWorld(), HealthBarWidgetClass));

			if (HealthBarWidgetInstance)
			{
				HealthBarWidgetInstance->AddToViewport();
				HealthBarWidgetInstance->UpdateHealthBar(CurrentHealth, MaxHealth);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HealthBarWidgetInstance OLUÞTURULAMADI!"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HealthBarWidgetClass ATANMADI!"));
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;



}

void AProje1Character::SwitchToMeleeMode()
{

	bIsRangedMode = false;

	// Kamera / hareket ayarlarý
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Kamera ayarlarýný melee moda döndürmek
	CameraBoom->TargetArmLength = 400.f;  
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 0.f);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("MELEE MODE"));
}

void AProje1Character::SwitchToRangedMode()
{
	bIsRangedMode = true;

	// Kamera / hareket ayarlarý
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;

	// Kamera ayarlarýný ranged moda alma
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->SocketOffset = FVector(0.f, 100.f, 75.f);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 10.f));

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("RANGED MODE"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProje1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Sol Mouse Týklamasýyla saldýrma
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AProje1Character::PerformAttack);

	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &AProje1Character::HealToFull);
	

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProje1Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProje1Character::Look);

	}

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AProje1Character::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AProje1Character::StopSprinting);



	PlayerInputComponent->BindAction("SwitchToMelee", IE_Pressed, this, &AProje1Character::SwitchToMeleeMode);
	PlayerInputComponent->BindAction("SwitchToRanged", IE_Pressed, this, &AProje1Character::SwitchToRangedMode);



}

void AProje1Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AProje1Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProje1Character::PerformAttack()
{
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			AnimInstance->Montage_Play(AttackMontage, 1.0f);
		}
	}
}

void AProje1Character::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprinting = true;

	if (SprintMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// Eðer çalmýyorsa baþlatmaya yarar
		if (!AnimInstance->Montage_IsPlaying(SprintMontage))
		{
			AnimInstance->Montage_Play(SprintMontage, 1.0f);
		}
	}
}

void AProje1Character::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprinting = false;

	if (SprintMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Stop(0.2f, SprintMontage);
	}
}

void AProje1Character::OnSwordOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Kilic bir seye carpti!"));

	AAIChar* HitEnemyAIChar = Cast<AAIChar>(OtherActor);
	if (HitEnemyAIChar)
	{
		float SwordDamage = 20.f;
		HitEnemyAIChar->TakeDamage(SwordDamage);
	}

	AAI4GOZ* HitEnemyAI4GOZ = Cast<AAI4GOZ>(OtherActor);
	if (HitEnemyAI4GOZ)
	{
		float SwordDamage = 20.f;
		HitEnemyAI4GOZ->TakeDamage(SwordDamage);
	}
}

void AProje1Character::TakeDamage(float DamageAmount)
{
	CurrentHealth -= DamageAmount;
	if (CurrentHealth < 0.f)
		CurrentHealth = 0.f;

	if (HealthBarWidgetInstance)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Can barý güncelleniyor!"));
		HealthBarWidgetInstance->UpdateHealthBar(CurrentHealth, MaxHealth);
	}

	if (CurrentHealth <= 0.f)
	{
		// Ölüm kodu
		Destroy();
	}
}


void AProje1Character::HealToFull()
{
	if (HealCharge > 0 && CurrentHealth < MaxHealth)
	{
		CurrentHealth = MaxHealth;
		HealCharge--;

		// Saðlýk barýný manuel güncelleme kodu
		OnHealthChanged();

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("Healed to full! Remaining charges: %d"),HealCharge));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("Cannot heal! Charges: %d, Health: %f"),HealCharge));
	}
}

void AProje1Character::OnHealthChanged() // Blueprint için yazdýðým kod
{
	if (HealthBarWidgetInstance)
	{
		HealthBarWidgetInstance->UpdateHealthBar(CurrentHealth, MaxHealth);
	}
}


