// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Animation/AnimMontage.h"
#include "AIChar.generated.h"

UCLASS()
class PROJE1_API AAIChar : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDistance = 180.0f;

	bool bHasAttacked = false;

	void AttackPlayer();  // Animasyonu oynatan fonksiyon

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

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
	AAIChar();

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

	// Oyuncuyu takip etmek için özel bir fonksiyon, zorla takip
	//void FollowPlayer();

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

	void FollowPlayer();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bPlayerSeen = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UFloatingDamageWidget> DamagePopupWidgetClass;


public:
	UFUNCTION(BlueprintCallable)
	void ShowFloatingDamage(float DamageAmount);


private:
	UPROPERTY(EditAnywhere, Category = "Death")  //Ölüm animasyonu için kodlar, animasyon editörden atanýyor. Deatils kýsmýnda "Death" yazýnca çýkar.
	UAnimMontage* DeathMontage = nullptr;

	bool bIsDead = false;

	FTimerHandle DeathDestroyTimer;

	void DestroyAfterDeath();

};
