// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SphereHordeGameMode.generated.h"

class ARadialActorsSpawner;

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

	// the number of the destroyed spheres
	int32	DestroyedSpheres;

	// the number of the spheres needed to be destoyed in order to start new wave
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "10", ClampMax = "30", UIMin = "30", UIMax = "30"))
	int32	DestroyedSpheresPerWave = 10;

	// updates the number of destroyed spheres
	void	UpdatedNubmerOfDestroyedSpheres();

protected:
	ARadialActorsSpawner* CreatedSheresSpawner;
};



