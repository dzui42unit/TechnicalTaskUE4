// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SphereHordeGameMode.generated.h"

class ARadialActorsSpawner;
class ASphereTarget;

UCLASS(minimalapi)
class ASphereHordeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASphereHordeGameMode();

	void BeginPlay() override;

	// radial actor spawner
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARadialActorsSpawner> SpheresSpawner;

	// the number of the spheres needed to be destoyed in order to start new wave
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "10", ClampMax = "15", UIMin = "10", UIMax = "30"), Category = "Gameplay")
	int32	DestroyedSpheresPerWave = 10;

	// the number of the spheres needed to be destoyed in order to start new wave
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "15", ClampMax = "30", UIMin = "15", UIMax = "30"), Category = "Gameplay")
	int32	ActorsPerWave = 15;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1500.0", ClampMax = "2000.0", UIMin = "1500.0", UIMax = "2000.0"), Category = "Gameplay")
	float	SpheresDistanceFromOrigin;

	// updates the number of destroyed spheres
	void	UpdatedNubmerOfDestroyedSpheres(ASphereTarget* TargetSphere);

private:
	// a spheres spawners 
	ARadialActorsSpawner* CreatedSpheresSpawner;

	// the number of the destroyed spheres
	int32	DestroyedSpheres;

	// checks if the sphere is in range of some distance from the spawner (1500.f) by default;
	bool	isInRangeFromTheOrigin(ASphereTarget* TargetSphere) const;
};



