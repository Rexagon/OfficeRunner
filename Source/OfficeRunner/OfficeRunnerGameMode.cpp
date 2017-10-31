// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OfficeRunnerGameMode.h"
#include "OfficeRunnerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOfficeRunnerGameMode::AOfficeRunnerGameMode()
{
	static ConstructorHelpers::FClassFinder<ACharacter> MainCharacterBP(TEXT("/Game/Blueprints/MainCharacter"));
	if (MainCharacterBP.Class != nullptr) {
		DefaultPawnClass = MainCharacterBP.Class;
	}
}
