// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RadialActorsSpawner.generated.h"

class UBoxComponent;
class ASphereTarget;
class AActor;
/*
	define the struct that describes the rules of the spawning process
	all the properties are axposed to the blueprint

	1. type of actors to spawn
	2. initial minimum distance between the actors
	3. initial amount of actors
	4. radius of spawn 
	5. step of changing the amount of spheres in percentages
	6. step of changing the spawn radius in percentages
*/

USTRUCT()
struct FSpawnRules
{
	GENERATED_BODY()

	// type of actors to spawn
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASphereTarget>	SpawnObject;

	// initial minimum distance between the actors
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "80.0", ClampMax = "160.0", UIMin = "80.0", UIMax = "160.0"))
	float	DistanceBetweenObjects = 80.f;

	// radius of the aread where to spawn object
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1500.0", ClampMax = "2000.0", UIMin = "1500.0", UIMax = "2000.0"))
	float	InnerSpawnRadius = 1500.f;

	// radius of the aread where to spawn object
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "2000.0", ClampMax = "3500.0", UIMin = "2000.0", UIMax = "3500.0"))
	float	OutterSpawnRadius = 2000.f;

	// the step of changing the amount of spheres in percentages
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "5.0", ClampMax = "100.0", UIMin = "5.0", UIMax = "100.0"))
	float	ActorsNbStep = 10.f;

	// the step of changing the spawn radius in percentages
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "5.0", ClampMax = "100.0", UIMin = "5.0", UIMax = "100.0"))
	float	SpawnRadiusStep = 5.f;

	// number of actor to spawn per wave
	int32	ActorsNb;

	// number of actors to spawn in inner radius
	int32	InnerRadiusActorsNb;

	// minimum scale of the actor to spawn
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.1", ClampMax = "0.9", UIMin = "0.1", UIMax = "0.9"))
	float	MinActorScale = 0.5f;

	// minimum scale of the actor to spawn
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01", ClampMax = "0.9", UIMin = "0.01", UIMax = "0.9"))
	float	ScaleActorStep = 0.1f;

	// boolean to swtich from the Gettting random point in the box extent
	// to the getting random reachable point in area
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Settings")
	bool	SpawnObjectsUnderPawn = false;
};

UCLASS()
class SPHEREHORDE_API ARadialActorsSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// the constructor that takes number of actors and number of inner radius actors for creation of spawner object
	ARadialActorsSpawner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// spawns the targets in the area
	void	SpawnTargetSpheres(int32 NbOfSpheres, const FVector& BoxExtent, float Radius);

	// checks if the distance of the actor is greater than some constant value
	bool	isActorFarFromSpawnedActors(ASphereTarget* SpawnedTarget, float Radius) const;

	// update the spawner parameters, number of spheres and radius
	void	StartNewWave();

	// initialize spawn actors number and spawn actors inside the inner radius
	void	Initialize(int32 ActorsNb, int32 InnerRadiusNb);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// struct that describes the spawn rules
	UPROPERTY(EditDefaultsOnly)
	FSpawnRules	SpawnRules;

	// sphere component to represent the area for spawning Target Spheres
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* OutterSpawnBoundingBox;	

	// sphere component to represent the area for spawning Target Spheres
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* InnerSpawnBoundingBox;

private:
	// the maximum scale of the spawned actor
	float		MaxActorScale;

	// current actor scale
	float		CurrentActorScale;

	// sets the spawner position, taking into account player pawn position
	void	SetSpawnerPosition();

	// the float value that represents Z adjustment of the of the spawner
	// relatively to the player pawn
	float	zOffset;

	// verctor that represents a final inner box extent
	FVector	BoxExtentInner;

	// verctor that represents a final outter box extent
	FVector	BoxExtentOutter;

	// updates the box extent of the inner and outter box
	void	UpdateZoffsetAndBoxHeight();

	// get number of the actors in the radius
	TArray<AActor*>	GetTargetsOnLevel() const;
};
