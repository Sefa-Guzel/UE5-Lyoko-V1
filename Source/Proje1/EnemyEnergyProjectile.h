// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyEnergyProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class PROJE1_API AEnemyEnergyProjectile : public AActor
{
	GENERATED_BODY()

public:
	AEnemyEnergyProjectile();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* Movement;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage = 10.f; //Normal saldýrýda verdiði hasar

	UPROPERTY(EditAnywhere, Category = "Combat")
	float LifeSeconds = 5.f;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
};
