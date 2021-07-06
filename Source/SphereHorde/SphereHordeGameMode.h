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

	void	UpdatedNubmerOfDestroyedSpheres();

protected:
	ARadialActorsSpawner* CreatedSheresSpawner;
};



