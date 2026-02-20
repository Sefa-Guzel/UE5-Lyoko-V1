// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryTower.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Proje1Character.h" // Ana karakter sýnýfýný tanýmak için
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Proje1GameMode.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AVictoryTower::AVictoryTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Mesh oluþtur
	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	RootComponent = TowerMesh;

	// Box Collision oluþtur
	VictoryTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("VictoryTrigger"));
	VictoryTrigger->SetupAttachment(RootComponent);
	VictoryTrigger->SetBoxExtent(FVector(250.f, 250.f, 250.f)); // Toplamda 500x500x500 kutu
	VictoryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	VictoryTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	VictoryTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void AVictoryTower::BeginPlay()
{
	Super::BeginPlay();

	// Overlap event baðlama
	VictoryTrigger->OnComponentBeginOverlap.AddDynamic(this, &AVictoryTower::OnOverlapBegin);
	
}

// Called every frame
void AVictoryTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVictoryTower::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProje1Character* Player = Cast<AProje1Character>(OtherActor);
	if (Player)
	{
		if (Player)
		{
			// Oyunu durdurmadan önce UI göster
			if (VictoryWidgetClass)
			{
				UUserWidget* VictoryWidget = CreateWidget<UUserWidget>(GetWorld(), VictoryWidgetClass);
				if (VictoryWidget)
				{
					VictoryWidget->AddToViewport();
				}
			}

			// Oyun durdurulsun
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}

	}
}

