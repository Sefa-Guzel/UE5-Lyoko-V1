// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AITank.generated.h"

UCLASS()
class PROJE1_API AAITank : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAITank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Referans oyuncu karakteri
	UPROPERTY()
	AActor* PlayerActor;

	// AI Kontrolcüsü referansý
	UPROPERTY()
	class AAIController* AIC_Ref;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Oyuncuyu takip etmek için özel bir fonksiyon
	void FollowPlayer();

};
