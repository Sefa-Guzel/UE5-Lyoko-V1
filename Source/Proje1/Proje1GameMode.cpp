// Copyright Epic Games, Inc. All Rights Reserved.

#include "Proje1GameMode.h"
#include "Proje1Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AProje1GameMode::AProje1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

