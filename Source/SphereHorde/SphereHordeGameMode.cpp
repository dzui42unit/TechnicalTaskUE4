// Copyright Epic Games, Inc. All Rights Reserved.

#include "SphereHordeGameMode.h"
#include "SphereHordeHUD.h"
#include "SphereHordeCharacter.h"
#include "RadialActorsSpawner.h"
#include "SphereTarget.h"
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

	// initialize the SpheresDistanceFromOrigin, that denotes the range from origin, within it the sphere should be counted as killed
	// and get a point for its destruction
	SpheresDistanceFromOrigin = 1500.f;
}

void ASphereHordeGameMode::BeginPlay()
{
	// Get player pawn, if the player pawn if not nullptr we get its location
	// to spawn a RadialActorsSpawner if RadialActorsSpawner is not nullptr
	// initialize its ActorsNb and InnerRadiusNb
	if (SpheresSpawner)
	{
		FTransform ActorTransform = FTransform((FVector::ZeroVector, FRotator::ZeroRotator));
		CreatedSpheresSpawner = GetWorld()->SpawnActorDeferred<ARadialActorsSpawner>(SpheresSpawner, ActorTransform);
		if (!CreatedSpheresSpawner)
		{
			UE_LOG(LogTemp, Warning, TEXT("FAILED to create CreatedSheresSpawner in SphereHordeGameMode"))
		}
		CreatedSpheresSpawner->Initialize(ActorsPerWave, DestroyedSpheresPerWave);
		CreatedSpheresSpawner->FinishSpawning(ActorTransform);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpheresSpawner is SphereHordeGameMode is NOT set"))
	}
}

// updates the number of destroyed spheres
void	ASphereHordeGameMode::UpdatedNubmerOfDestroyedSpheres(ASphereTarget* TargetSphere)
{
	// check if the destroyed sphere is in range from the spawn origin
	if (isInRangeFromTheOrigin(TargetSphere))
	{
		DestroyedSpheres++;
		UE_LOG(LogTemp, Warning, TEXT("Number of the destroyed spheres: %d"), DestroyedSpheres)
	}
	// check if we have destroyed needed number of the spheres to finish the wave
	// and start new wave if the number is reached
	if ((DestroyedSpheres % DestroyedSpheresPerWave) == 0 && (DestroyedSpheres != 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("TIME TO UPDATE WAVE"))
		CreatedSpheresSpawner->StartNewWave();
	}
}

// checks if the sphere is in range of some distance from the spawner (1500.f) by default;
bool ASphereHordeGameMode::isInRangeFromTheOrigin(ASphereTarget* TargetSphere) const
{
	// get the position of twoo objects
	FVector TargetSpherePosition = TargetSphere->GetActorLocation();
	FVector SpawnerPosition = CreatedSpheresSpawner->GetActorLocation();

	// calculate the distance, if the distance is smaller than predefined radius - we returrn true and say
	// that sphere is in range
	float distance = (TargetSpherePosition - SpawnerPosition).Size();
	// we take a half of disntace - as a radius
	UE_LOG(LogTemp, Warning, TEXT("%s, %s, distance was %f"), *TargetSpherePosition.ToString(), *SpawnerPosition.ToString(), distance)
	if (distance <= (SpheresDistanceFromOrigin))
	{
		return true;
	}

	return false;
}
