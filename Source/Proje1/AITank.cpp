// Fill out your copyright notice in the Description page of Project Settings.


#include "AITank.h"
#include <AIController.h>
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAITank::AAITank()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAITank::BeginPlay()
{
	Super::BeginPlay();

	AIC_Ref = Cast<AAIController>(GetController());

	PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
}

// Called every frame
void AAITank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Eðer oyuncu karakteri mevcutsa takip et
	if (PlayerActor && AIC_Ref)
	{
		FollowPlayer();
	}

}

// Oyuncuyu takip etmek için özel fonksiyon
void AAITank::FollowPlayer()
{
	if (!PlayerActor || !AIC_Ref) return;

	// Oyuncunun pozisyonunu al
	FVector PlayerLocation = PlayerActor->GetActorLocation();

	// AI'nin oyuncuya ulaþmasý gereken minimum mesafe
	float StopDistance = 1500.0f; // 150 birimlik bir mesafede duracak

	// Oyuncuya olan mesafeyi kontrol et
	float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerLocation);

	if (DistanceToPlayer > StopDistance)
	{
		// Eðer mesafe büyükse, hareket etmeye devam et
		AIC_Ref->MoveToLocation(PlayerLocation, StopDistance);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Oyuncu Takip Ediliyor!"));
	}
	else
	{
		// Eðer mesafe yeterince yakýnsa, dur
		AIC_Ref->StopMovement();
	}
	//if (!PlayerActor || !AIC_Ref) return;

	// Oyuncunun pozisyonunu al
	//FVector PlayerLocation = PlayerActor->GetActorLocation();

	// AI karakterini oyuncuya doðru hareket ettir
	//AIC_Ref->MoveToLocation(PlayerLocation);

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Oyuncu Takip Ediliyor!"));
}

// Called to bind functionality to input
void AAITank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

