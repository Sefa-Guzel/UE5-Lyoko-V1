// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingDamageWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJE1_API UFloatingDamageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageText;

	void SetDamageText(float DamageAmount);
};
