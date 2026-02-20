// Fill out your copyright notice in the Description page of Project Settings.


#include "AI4GOZ.h"
#include <AIController.h>
#include "Proje1Character.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidget.h"
#include "Blueprint/UserWidget.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Components/SceneComponent.h"
#include "EnemyEnergyProjectile.h"
#include "DrawDebugHelpers.h"
#include "FloatingDamageWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"



// Sets default values
AAI4GOZ::AAI4GOZ()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// AIPerception bileþenini oluþtur
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// Görüþ ayarlarý
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 4000.0f; //Görmenin uzaklýðý
	SightConfig->LoseSightRadius = 4200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 180.0f; //Görmenin açýsý
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAI4GOZ::OnPerceptionUpdated);

	PrimaryActorTick.bCanEverTick = true;
	NewHealth = 100.f; // Baþlangýç saðlýðý


	FirePointFront = CreateDefaultSubobject<USceneComponent>(TEXT("FirePointFront"));
	FirePointFront->SetupAttachment(GetMesh()); //Gözdeki lazerleri oluþturdum

	EyePointFront = CreateDefaultSubobject<USceneComponent>(TEXT("EyePointFront"));
	EyePointFront->SetupAttachment(GetMesh());

	EyePointBack = CreateDefaultSubobject<USceneComponent>(TEXT("EyePointBack"));
	EyePointBack->SetupAttachment(GetMesh());

	EyePointLeft = CreateDefaultSubobject<USceneComponent>(TEXT("EyePointLeft"));
	EyePointLeft->SetupAttachment(GetMesh());

	EyePointRight = CreateDefaultSubobject<USceneComponent>(TEXT("EyePointRight"));
	EyePointRight->SetupAttachment(GetMesh());


}

// Called when the game starts or when spawned
void AAI4GOZ::BeginPlay()
{
	Super::BeginPlay();

	AIC_Ref = Cast<AAIController>(GetController());
	if (AIC_Ref)
	{

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("AIController YOK!!!"));
	}

	PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	// Oyuncu karakterini buluyor
	PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerActor)
	{

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("PlayerActor bulunamadý!"));
	}

	NewHealth = MaxHealth;


	
	
	ProjectileCooldown = 0.f;

	// Special attack timer: her 10 sn’de bir dene
	GetWorldTimerManager().SetTimer(
		LaserSpinTimerHandle,
		this,
		&AAI4GOZ::StartLaserSpin,
		LaserSpinInterval,
		true
	);

}

// Called every frame
void AAI4GOZ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Eðer oyuncu mevcutsa takip et (Görmeden direkt takip)
	//if (PlayerActor && AIC_Ref)
	//{
	//	FollowPlayer();
	//}

	// AI takip etme kodu
	if (bPlayerSeen && PlayerActor && AIC_Ref && AttackState == EAI4GOZAttackState::NormalFire)
	{
		FollowPlayer();
	}

	// Cooldown düþür
	if (ProjectileCooldown > 0.f)
	{
		ProjectileCooldown -= DeltaTime;
	}

	// Normal projectile saldýrýsý
	if (AttackState == EAI4GOZAttackState::NormalFire && bPlayerSeen && PlayerActor && AIC_Ref)
	{
		TryFireProjectile();
	}

	// Laser spin modunda 360 derece dön
	if (AttackState == EAI4GOZAttackState::LaserSpin)
	{
		LaserSpinElapsed += DeltaTime;

		const float SpinRateDegPerSec = 360.0f / FMath::Max(LaserSpinDuration, 0.01f);
		SpinYawAccum += SpinRateDegPerSec * DeltaTime; // 360 dereceyi tamamlamýyordu ondan ekledim
		//AddActorLocalRotation(FRotator(0.f, SpinRateDegPerSec * DeltaTime, 0.f));  //Pürüzsüz dönüþ olsun diye aþaðýdakiyle deðiþtirdim.

		FRotator Current = GetActorRotation();                    
		//FRotator Target = Current + FRotator(0.f, SpinRateDegPerSec * DeltaTime, 0.f);  
		// Hedef rotasyon: baþlangýç + biriken yaw
		FRotator Target = SpinStartRot;
		Target.Yaw += SpinYawAccum;

		// Yumuþatmaya yarýyor (takýlma olmasýn diye):
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), Target, DeltaTime, 10.0f));

		SetActorRotation(FMath::RInterpTo(
			Current,
			Target,
			DeltaTime,
			10.0f   // Interp speed (8–12 arasý çok iyiymiþ)
		));


		// 360 tamamlanýnca bitir
		if (SpinYawAccum >= 360.0f)
		{
			EndLaserSpin();
		}
	}


}

void AAI4GOZ::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (Actor == PlayerActor) // Oyuncu algýlandý mý?
		{
			bPlayerSeen = true; // Artýk oyuncuyu sonsuza kadar görmüþ say
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Oyuncu Algýlandý!"));
			FollowPlayer();
		}
	}
}

// Oyuncuyu takip etmek için özel fonksiyon
void AAI4GOZ::FollowPlayer()
{
	if (!PlayerActor || !AIC_Ref) return;

	// Oyuncunun pozisyonunu al
	FVector PlayerLocation = PlayerActor->GetActorLocation();

	// AI'nin oyuncuya ulaþmasý gereken minimum mesafe (Ateþ etmesi için)
	float StopDistance = 1000.0f; // 1000 birimlik bir mesafede duracak

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
	
}

// Called to bind functionality to input
void AAI4GOZ::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAI4GOZ::TakeDamage(float DamageAmount)
{
	NewHealth -= DamageAmount;

	if (NewHealth < 0.f)
		NewHealth = 0.f;


	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Incoming Damage: %f"), DamageAmount));


	UpdateHealthBarUI(NewHealth, MaxHealth);

	if (NewHealth <= 0.f)
	{
		// Oyuncuya can yenileme hakký ver
		AProje1Character* Player = Cast<AProje1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			Player->HealCharge++;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("HealCharge arttý! Yeni deðer: %d"), DamageAmount));
		}



		Destroy();

	}

}

void AAI4GOZ::TryFireProjectile()
{
	if (ProjectileCooldown > 0.f) return;
	if (!ProjectileClass) return;
	if (!PlayerActor) return;

	// AI durmuþ bile olsa projectile atabilir
	const FVector SpawnLoc = FirePointFront ? FirePointFront->GetComponentLocation() : GetActorLocation() + GetActorForwardVector() * 100.f;

	// Player'a doðru yönelmeyi saðlýyor
	const FVector ToPlayer = (PlayerActor->GetActorLocation() - SpawnLoc).GetSafeNormal();
	const FRotator SpawnRot = ToPlayer.Rotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	AEnemyEnergyProjectile* Proj = GetWorld()->SpawnActor<AEnemyEnergyProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
	if (Proj)
	{
		// Buradan da Damage ayarlanabilir; basit olsun diye projectile'de sabit býraktým. Dursun.
	}

	ProjectileCooldown = ProjectileFireInterval; // 1 sn
}


void AAI4GOZ::StartLaserSpin()
{
	// Oyuncu görülmediyse special atak yapmasýn
	if (!bPlayerSeen || !PlayerActor || !AIC_Ref) return;

	// Zaten lazerdeysek tekrar baþlatma
	if (AttackState == EAI4GOZAttackState::LaserSpin) return;

	AttackState = EAI4GOZAttackState::LaserSpin;
	LaserSpinElapsed = 0.f;

	SpinYawAccum = 0.f;
	SpinStartRot = GetActorRotation(); //360 derece dönmeye zorlar

	// Hareketi durdur
	AIC_Ref->StopMovement();
	GetCharacterMovement()->StopMovementImmediately();

	// Lazer damage tick (0.1 sn)
	GetWorldTimerManager().SetTimer(
		LaserTickTimerHandle,
		this,
		&AAI4GOZ::LaserDamageTick,
		LaserTickInterval,
		true
	);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

}


void AAI4GOZ::LaserDamageTick()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("LASER TICK RUNNING")));


	if (!PlayerActor) return;

	auto DoEyeTrace = [&](USceneComponent* EyePoint)
	{
			if (!EyePoint) return;

			const FVector Start = EyePoint->GetComponentLocation();
			const FVector End = Start + EyePoint->GetForwardVector() * LaserRange;

			FCollisionQueryParams Params;
			//Params.AddIgnoredActor(this);

			Params.bReturnPhysicalMaterial = false;
			Params.AddIgnoredActor(this);

			FHitResult Hit;
			const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);

			// Debug lazer çizgi (istersen geçici açýk tut)
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, LaserTickInterval, 0, 4.f);

			if (bHit)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor) return;

				// Owner üzerinden player'a ulaþ
				AProje1Character* Player = Cast<AProje1Character>(HitActor->GetOwner());
				
				
				if (!Player)
				{
					Player = Cast<AProje1Character>(HitActor);
				}


				if (Player)
				{
					const float DamageThisTick = LaserDPS * LaserTickInterval;
					Player->TakeDamage(DamageThisTick);
				}
			}


		

	};

	DoEyeTrace(EyePointFront);
	DoEyeTrace(EyePointBack);
	DoEyeTrace(EyePointLeft);
	DoEyeTrace(EyePointRight);
}


void AAI4GOZ::EndLaserSpin()
{
	AttackState = EAI4GOZAttackState::NormalFire;

	// Lazer tick’i kapat
	GetWorldTimerManager().ClearTimer(LaserTickTimerHandle);

	// Normal atýþýn cooldown'ýný hafif resetlemek için:
	ProjectileCooldown = 0.2f;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

}

void AAI4GOZ::ShowFloatingDamage(float DamageAmount) //Popup kodlarý klasik
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, TEXT("*************"));

	if (!DamagePopupWidgetClass) return;

	// Doðrudan FloatingDamageWidget olarak cast edilecek þekilde widget oluþturuluyor
	UFloatingDamageWidget* DamageWidget = CreateWidget<UFloatingDamageWidget>(GetWorld(), DamagePopupWidgetClass);
	if (DamageWidget)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Damage Widget Created"));

		DamageWidget->AddToViewport();

		// C++ fonksiyonunu çaðýrýyoruz (string deðil)
		DamageWidget->SetDamageText(DamageAmount);

		FVector WorldLocation = GetActorLocation() + FVector(0, 0, 150); 
		FVector2D ScreenLocation;
		UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0), WorldLocation, ScreenLocation);

		DamageWidget->SetPositionInViewport(ScreenLocation);
	}

	// 1 saniye sonra widget'ý kaldýr
	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, [DamageWidget]()
		{
			DamageWidget->RemoveFromParent();
		}, 1.0f, false);


}

