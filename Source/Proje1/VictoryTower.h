// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VictoryTower.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJE1_API AVictoryTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVictoryTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Kule mesh'i
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TowerMesh;

	// Çarpýþma kutusu
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* VictoryTrigger;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> VictoryWidgetClass;


	// Fonksiyon: karakter çarpýnca çalýþacak
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
