// Fill out your copyright notice in the Description page of Project Settings.


#include "AIChar.h"
#include "Proje1Character.h"
#include <AIController.h>
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidget.h"
#include "Blueprint/UserWidget.h"
#include "FloatingDamageWidget.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

// Sets default valuese
AAIChar::AAIChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// AIPerception bileþenini oluþtur
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// Görüþ ayarlarý
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 4000.0f; // Görüþ mesafesi
	SightConfig->LoseSightRadius = 4200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 190.0f;  //Görüþ açýsý
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIChar::OnPerceptionUpdated);

	PrimaryActorTick.bCanEverTick = true;
	NewHealth = 100.f; // Baþlangýç saðlýðý



}

// Called when the game starts or when spawned
void AAIChar::BeginPlay()
{
	Super::BeginPlay();
	
	if (bIsDead) return;


	// AI Controller referansýný al
	AIC_Ref = Cast<AAIController>(GetController());
	if (AIC_Ref)
	{
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("AIController YOK!!!"));
	}

	// Oyuncu karakterini bul
	PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerActor)
	{
		//Aþaðýdaki kodun aynýsýný TEXT'i deðiþtirip yazarsý lazýmsa
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("PlayerActor bulunamadý!"));
	}

	//sürekli saldýrý
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AAIChar::OnAttackMontageEnded);
	}
	
	NewHealth = MaxHealth;

}

// Called every frame
void AAIChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead) return;


	if (bPlayerSeen && PlayerActor && AIC_Ref)
	{
		float Distance = FVector::Dist(GetActorLocation(), PlayerActor->GetActorLocation());

		if (Distance <= AttackDistance)
		{
			if (!bHasAttacked)
			{
				AttackPlayer(); //if kodlarýnda biraz gariplik var ama çalýþýyo þimdilik dokunma, normal saldýrý
			}
		}
		else
		{
			bHasAttacked = false;

			// Takip etmeye devam et
			FollowPlayer();
		}
	}
	
	// Eðer oyuncu karakteri mevcutsa takip et (Görmeden direkt takip)(Zorla takip eski kod dursun)
	//if (PlayerActor && AIC_Ref)
	//{
	//	FollowPlayer();
	//}

}

void AAIChar::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (bIsDead) return;


	for (AActor* Actor : UpdatedActors)
	{
		if (Actor == PlayerActor) // Oyuncu algýlandý mý?
		{
			bPlayerSeen = true; // Oyuncuyu sonsuza kadar görmüþ say
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Oyuncu Algýlandý!"));
			FollowPlayer();
		}
	}
}

// Oyuncuyu takip etmek için özel fonksiyon
void AAIChar::FollowPlayer()
{
	if (bIsDead) return;


	if (!PlayerActor || !AIC_Ref) return;

	FVector PlayerLocation = PlayerActor->GetActorLocation();
	float StopDistance = 150.0f;
	float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerLocation);

	if (DistanceToPlayer > StopDistance)
	{
		// Karakterin yönünü manuel olarak hedefe döndür
		FVector Direction = (PlayerLocation - GetActorLocation()).GetSafeNormal();
		FRotator NewRotation = Direction.Rotation();
		SetActorRotation(NewRotation);

		AIC_Ref->MoveToLocation(PlayerLocation, StopDistance);


	}
	else
	{
		AIC_Ref->StopMovement();
	}
}
// Called to bind functionality to input
void AAIChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIChar::AttackPlayer()
{
	if (bIsDead) return;


	// Test
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Attack Triggered!"));
	if (!AttackMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		AnimInstance->Montage_Play(AttackMontage); //Animasyonlar Blueprint kýsmýndan eklendi
		bHasAttacked = true;

		// Hemen hasar ver
		if (PlayerActor)
		{
			AProje1Character* Player = Cast<AProje1Character>(PlayerActor);
			if (Player)
			{
				float Damage = 10.f;
				Player->TakeDamage(Damage);
			}
		}
	}
}

void AAIChar::TakeDamage(float DamageAmount)
{
	NewHealth -= DamageAmount;

	if (NewHealth < 0.f)
		NewHealth = 0.f;

	ShowFloatingDamage(DamageAmount);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Incoming Damage: %f"), DamageAmount));


	UpdateHealthBarUI(NewHealth, MaxHealth);

	if (NewHealth <= 0.f)
	{
		if (bIsDead) return;
		bIsDead = true;

		// Oyuncuya can yenileme hakký veriyor
		AProje1Character* Player = Cast<AProje1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			Player->HealCharge++;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("HealCharge arttý! Yeni deðer: %d"), DamageAmount));
		}
		
		// Ölüm animasyonu oynat
		float DestroyDelay = 0.3f; // fallback
		if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance()) //GetMesh iki defa yazmayýnca çalýþmadý, animasyonu oyunda ekledik BP
		{
			UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
			AnimInst->StopAllMontages(0.1f);

			// Montage_Play dönüþü: oynatýlan süre (genelde montage length / playrate)
			DestroyDelay = AnimInst->Montage_Play(DeathMontage, 1.0f);
			DestroyDelay = FMath::Max(0.1f, DestroyDelay - 0.15f);
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Death montage played, delay=%f"), DestroyDelay)); //animasyon çalýþýyor mu diye kontrol
			if (DestroyDelay <= 0.f)
			{
				DestroyDelay = 0.3f;
			}
		}

		// Ölürken bir daha hasar/overlap tetiklenmesin diye kapsülü kapat (ama kod çalýþmadý hata veriyor ilginç BAKILACAK!!!)
		// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Süre dolunca Destroy
		GetWorldTimerManager().SetTimer(DeathDestroyTimer, this, &AAIChar::DestroyAfterDeath, DestroyDelay, false);

		return;
		
		
	}

}

void AAIChar::DestroyAfterDeath()
{
	Destroy(); //animasyondan sonra karakteirn yok olmasýný saðlýyor klasik.
}

//void AAIChar::UpdateHealthBarUI(float InNewHealth, float InMaxHealth)
//{
//	if (HealthBarWidget)
//	{
//		UHealthBarWidget* HealthWidget = Cast<UHealthBarWidget>(HealthBarWidget->GetUserWidgetObject());
//		if (HealthWidget)
//		{
//			HealthWidget->SetHealth(InNewHealth / InMaxHealth);
//		}
//	}
//}


void AAIChar::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		bHasAttacked = false; // Animasyon bitince tekrar saldýrabilir
	}
}

void AAIChar::ShowFloatingDamage(float DamageAmount)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, TEXT("*************"));

	if (!DamagePopupWidgetClass) return;

	// Doðrudan FloatingDamageWidget olarak cast edilecek þekilde widget oluþturmaya yarýyor.
	UFloatingDamageWidget* DamageWidget = CreateWidget<UFloatingDamageWidget>(GetWorld(), DamagePopupWidgetClass);
	if (DamageWidget) //Kodda NOT olunca çalýþmýyor optimize edilebilir gibi BAKILACAK!!!
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




