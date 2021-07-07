// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RadialActorsSpawner.generated.h"

class ASphereTarget;
class UBoxComponent;

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

class UBoxComponent;

USTRUCT()
struct FSpawnRules
{
	GENERATED_BODY()

	// type of actors to spawn
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASphereTarget>	SpawnObject;

	// initial minimum distance between the actors
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "80.0", ClampMax = "160.0", UIMin = "80.0", UIMax = "160.0"))
	float	DistanceBetweenObjects;

	// radius of the aread where to spawn object
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "100", ClampMax = "3500", UIMin = "100", UIMax = "3500"))
	float	InnerSpawnRadius;

	// radius of the aread where to spawn object
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "100", ClampMax = "3500", UIMin = "100", UIMax = "3500"))
	float	OutterSpawnRadius;

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
	void	SpawnTargetSpheres(int32 NbOfSpheres, FVector BoxExtent, float Radius);

	// returns the number of spawned actors
	int32	GetNumberOfSpawnedActors() const;

	// checks if the distance of the actor is greater than some constant value
	bool	isActorFarFromSpawnedActors(ASphereTarget* SpawnedTarget, float Radius) const;

	// update the spawner parameters, number of spheres and radius
	void	StartNewWave();

	// sets the spawner position, taking into account player pawn position
	void	SetSpawnerPosition();

	// initialize spawn actors number and spawn actors inside the inner radius
	void	Initialize(int32 ActorsNb, int32 InnerRadiusNb);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// struct that describes the spawn rules
	UPROPERTY(EditDefaultsOnly)
	FSpawnRules	SpawnRules;

	// sphere component to represent the area for spawning Target Spheres
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* OutterSpawnBoundingBox;	

	// sphere component to represent the area for spawning Target Spheres
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* InnerSpawnBoundingBox;

	// array that holds all target objects
	TArray<ASphereTarget *>	TargetSpheres;

private:
	// update the array of spawned actors
	void		UpdatedActorsArray();
};
