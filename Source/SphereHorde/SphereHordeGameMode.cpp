// Copyright Epic Games, Inc. All Rights Reserved.

#include "SphereHordeGameMode.h"
#include "SphereHordeHUD.h"
#include "SphereHordeCharacter.h"
#include "RadialActorsSpawner.h"
#include "UObject/ConstructorHelpers.h"

ASphereHordeGameMode::ASphereHordeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASphereHordeHUD::StaticClass();

	// initialize counter of DestroyedSpheres
	DestroyedSpheres = 0;
}

void ASphereHordeGameMode::BeginPlay()
{
	/*
		Get player pawn, if the player pawn if not nullptr we get its location
		to spawn a RadialActorsSpawner if  RadialActorsSpawner is not nullptr
	*/
	if (SpheresSpawner)
	{
		CreatedSheresSpawner = GetWorld()->SpawnActor<ARadialActorsSpawner>(SpheresSpawner, FVector::ZeroVector, FRotator::ZeroRotator);
		if (CreatedSheresSpawner)
		{
			// set the position of the spawner
			CreatedSheresSpawner->SetSpawnerPosition();
			// spawn targets
			CreatedSheresSpawner->SpawnTargetSpheres();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FAILED to create CreatedSheresSpawner in SphereHordeGameMode"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpheresSpawner is SphereHordeGameMode is NOT set"))
	}
}

void	ASphereHordeGameMode::UpdatedNubmerOfDestroyedSpheres()
{
	DestroyedSpheres++;
	//UE_LOG(LogTemp, Warning, TEXT("Number of the destroyed spheres: %d"), DestroyedSpheres)
	//UE_LOG(LogTemp, Warning, TEXT("Number of the spheres left in array: %d"), CreatedSheresSpawner->GetNumberOfSpawnedActors())

	if ((DestroyedSpheres % DestroyedSpheresPerWave) == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("TIME TO UPDATE WAVE"))
		CreatedSheresSpawner->UpdateSpawnerParameters();
	}

}