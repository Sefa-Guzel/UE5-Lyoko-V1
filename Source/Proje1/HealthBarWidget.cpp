// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"
#include "Components/ProgressBar.h"

void UHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Baþlangýçta %100 saðlýk vermeye yarýyor oyun içinden de deðiþtirilebilir.
    UpdateHealthBar(1.0f, 1.0f);
}

void UHealthBarWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
	if (HealthProgressBar && MaxHealth > 0)
	{
		HealthProgressBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void UHealthBarWidget::SetHealth(float HealthPercent)
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetPercent(HealthPercent);
    }
}


