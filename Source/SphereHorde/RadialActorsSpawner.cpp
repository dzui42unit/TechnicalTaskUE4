// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "RadialActorsSpawner.h"

// Sets default values
ARadialActorsSpawner::ARadialActorsSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create a bounding box components, it defines the area where to pick a location for spawn,
	// set it as root component
	// the radius is defined by the SpawnRules
	SpawnBoundingBox = CreateDefaultSubobject<UBoxComponent>("Spawn Box");
	if (SpawnBoundingBox)
	{
		SpawnBoundingBox->SetCollisionProfileName("NoCollision");
		SpawnBoundingBox->SetBoxExtent(FVector(SpawnRules.SpawnRadius, SpawnRules.SpawnRadius, SpawnRules.SpawnRadius));
		SetRootComponent(SpawnBoundingBox);
	}
}

// Called when the game starts or when spawned
void ARadialActorsSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARadialActorsSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// returns the radius of the spawner actor
float ARadialActorsSpawner::GetSpawnerRadius() const
{
	return SpawnRules.SpawnRadius;
}

// spawns a N number of target spheres
void ARadialActorsSpawner::SpawnTargetSpheres()
{
	if (!SpawnRules.SpawnObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnRules.SpawnObject is NOT set"))
		return;
	}

	// run the loop until needed number of the targets will not be created
	int32 spawnedTargetsNb = 0;
	while (spawnedTargetsNb < SpawnRules.ActorsNb)
	{
		// get a random point in the bounding sphere and spawn an TargetSphere
		// we specify actor spawn parameters to nake it take into account collision with other objects on scene
		// Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found
		FActorSpawnParameters SpawnActorParameters;
		SpawnActorParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		FVector SpawnPointLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), SpawnBoundingBox->Bounds.BoxExtent);
		ASphereTarget* CreatedTarget = GetWorld()->SpawnActor<ASphereTarget>(SpawnRules.SpawnObject, SpawnPointLocation, FRotator::ZeroRotator, SpawnActorParameters);
		if (CreatedTarget)
		{
			// add created target to the array of targets
			spawnedTargetsNb++;
			TargetSpheres.Add(CreatedTarget);
		}
	}
}

