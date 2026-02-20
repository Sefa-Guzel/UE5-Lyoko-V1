// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class PROJE1_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // Can çubuðunu güncelleyecek fonksiyon
    UFUNCTION(BlueprintCallable)
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthProgressBar;

public:

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetHealth(float HealthPercent);

protected:
    virtual void NativeConstruct() override;
};
