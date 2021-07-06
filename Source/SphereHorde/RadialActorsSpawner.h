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
	float	DistanceBetweenObjects = 80.f;

	// initial amount of actors
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "10", ClampMax = "20", UIMin = "10", UIMax = "20"))
	int32	ActorsNb = 10;

	// radius of the aread where to spawn object
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1500", ClampMax = "2000", UIMin = "1500", UIMax = "2000"))
	float	SpawnRadius = 1500.f;
};

UCLASS()
class SPHEREHORDE_API ARadialActorsSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadialActorsSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// struct that describes the spawn rules
	UPROPERTY(EditDefaultsOnly)
	FSpawnRules	SpawnRules;

	// sphere component to represent the area for spawning Target Spheres
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* SpawnBoundingBox;

	TArray<ASphereTarget *>	TargetSpheres;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void		SpawnTargetSpheres();

	float		GetSpawnerRadius() const;

};
