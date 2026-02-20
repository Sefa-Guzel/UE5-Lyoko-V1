// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingDamageWidget.h"
#include "Components/TextBlock.h"

void UFloatingDamageWidget::SetDamageText(float DamageAmount)
{
	if (DamageText) 
	{
		DamageText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), DamageAmount)));
	}
}
