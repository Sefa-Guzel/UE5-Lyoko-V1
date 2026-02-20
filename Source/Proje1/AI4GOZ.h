// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnemyEnergyProjectile.h"
#include "AI4GOZ.generated.h"

UENUM(BlueprintType)
enum class EAI4GOZAttackState : uint8
{
	NormalFire,
	LaserSpin
};

UCLASS()
class PROJE1_API AAI4GOZ : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float NewHealth;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateHealthBarUI(float InNewHealth, float InMaxHealth);

	// Saðlýk barý bileþeni (ekranda gösterilecek)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HealthBarWidget;

public:
	// Sets default values for this character's properties
	AAI4GOZ();

	// Hasar alma fonksiyonu
	void TakeDamage(float DamageAmount);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Referans oyuncu karakteri
	//UPROPERTY()
	//AActor* PlayerActor;

	// AI Kontrolcüsü referansý
	//UPROPERTY()
	//class AAIController* AIC_Ref;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	AAIController* AIC_Ref;

	UPROPERTY()
	AActor* PlayerActor;

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	// Oyuncuyu zorla takip
	void FollowPlayer();


public:
	// --- Combat / Ranged ---
	UPROPERTY(EditAnywhere, Category = "Combat|Projectile")
	TSubclassOf<AEnemyEnergyProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Combat|Projectile")
	float ProjectileDamage = 10.f;		// Toplarýn hasarý

	UPROPERTY(EditAnywhere, Category = "Combat|Projectile")
	float ProjectileFireInterval = 1.0f;

	// Special attack
	UPROPERTY(EditAnywhere, Category = "Combat|Laser")
	float LaserSpinInterval = 12.0f;     // her 10 saniye

	UPROPERTY(EditAnywhere, Category = "Combat|Laser")
	float LaserSpinDuration = 5.0f;      // 360 dereceyi kaç saniyede dönsün

	UPROPERTY(EditAnywhere, Category = "Combat|Laser")
	float LaserRange = 2500.0f;			// Lazerin uzunluðu (Galiba, kontrol et)

	UPROPERTY(EditAnywhere, Category = "Combat|Laser")
	float LaserDPS = 40.0f;              // Lazerin saniyelik hasarý

	UPROPERTY(EditAnywhere, Category = "Combat|Laser")
	float LaserTickInterval = 0.1f;      // lazer hasar kontrol sýklýðý

private:
	// Fire points (Blueprint’te gözlere yerleþtirdim)
	UPROPERTY(VisibleAnywhere, Category = "Combat|Points", meta = (AllowPrivateAccess = "true"))
	USceneComponent* FirePointFront;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Points", meta = (AllowPrivateAccess = "true"))
	USceneComponent* EyePointFront;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Points", meta = (AllowPrivateAccess = "true"))
	USceneComponent* EyePointBack;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Points", meta = (AllowPrivateAccess = "true"))
	USceneComponent* EyePointLeft;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Points", meta = (AllowPrivateAccess = "true"))
	USceneComponent* EyePointRight;

	// State & zamanlayýcý
	UPROPERTY(VisibleAnywhere, Category = "Combat|State", meta = (AllowPrivateAccess = "true"))
	EAI4GOZAttackState AttackState = EAI4GOZAttackState::NormalFire;

	float ProjectileCooldown = 0.f;

	FTimerHandle LaserSpinTimerHandle;
	FTimerHandle LaserTickTimerHandle;

	float LaserSpinElapsed = 0.f;

	float SpinYawAccum = 0.f;
	FRotator SpinStartRot;


	// fonkisyonlar
	void TryFireProjectile();
	void StartLaserSpin();
	void EndLaserSpin();
	void LaserDamageTick();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bPlayerSeen = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UFloatingDamageWidget> DamagePopupWidgetClass;


public:
	UFUNCTION(BlueprintCallable)
	void ShowFloatingDamage(float DamageAmount);


};
