// Copyright Epic Games, Inc. All Rights Reserved.

#include "SphereHordeGameMode.h"
#include "SphereHordeHUD.h"
#include "SphereHordeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASphereHordeGameMode::ASphereHordeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASphereHordeHUD::StaticClass();
}
