// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyEnergyProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Proje1Character.h"

AEnemyEnergyProjectile::AEnemyEnergyProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->InitSphereRadius(25.f);  //küre boyutu
	Collision->SetUseCCD(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 2000.f; //Kürenin gelme hýzý
	Movement->MaxSpeed = 2000.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;
	Movement->ProjectileGravityScale = 0.f;


	Collision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyEnergyProjectile::OnOverlap);
}

void AEnemyEnergyProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSeconds);
}

void AEnemyEnergyProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	// Player hasarý
	if (AProje1Character* Player = Cast<AProje1Character>(OtherActor))
	{
		Player->TakeDamage(Damage);
		Destroy(); //klasik yok olma
		return;
	}

	// Dünya / baþka bir þeye çarptýysa yok et (belki kullanýrýz)
	// Destroy();
}

