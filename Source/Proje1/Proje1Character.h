// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.h"
#include "Proje1Character.generated.h"


UCLASS(config=Game)
class AProje1Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Koþma sistemi için gereken deðiþkenler
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 500.0f; //Normal koþu

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 1000.0f; // Hýzlý koþu

	// Koþma fonksiyonlarý
	void StartSprinting();
	void StopSprinting();

protected:
	// Can deðeri
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Health11;

public:

	// Widget referansý
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UHealthBarWidget> HealthBarWidgetClass;

	UPROPERTY()
	UHealthBarWidget* HealthBarWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	int32 HealCharge = 1; // Oyuna 1 defa can yenileme hakký ile baþlamayý saðlayan kod

	// Can yenileme fonksiyonu
	void HealToFull();

	//Can Güncellemeyi mauel bildiriyor, eeðer yapmazsak AIChar saldrýdýktan sonra güncelleniyor o yüzden var
	void OnHealthChanged();

	UPROPERTY()
	bool bIsSprinting = false;


public:
	// Hasar alma fonksiyonu
	void TakeDamage(float DamageAmount);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsRangedMode = false;

	UFUNCTION()
	void SwitchToMeleeMode(); //mod deðiþtirme kodlarý (1'e bas)

	UFUNCTION()
	void SwitchToRangedMode();	//mod deðiþtirme kodlarý (2'ye bas)





	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackMontage;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SprintMontage;

	UFUNCTION()
	void PerformAttack();

public:
	AProje1Character();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	// Kýlýç çarpýþma kutusu
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent* SwordHitBox;

public:
	// Kýlýcýn çarpýþma fonksiyonu
	UFUNCTION()
	void OnSwordOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

