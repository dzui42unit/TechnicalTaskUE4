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

	SpawnRules.ActorsNb = 15;
	SpawnRules.SpawnRadius = 1000.f;
	SpawnRules.DistanceBetweenObjects = 80.f;

	// create a bounding box components, it defines the area where to pick a location for spawn,
	// set it as root component
	// the radius is defined by the SpawnRules
	SpawnBoundingBox = CreateDefaultSubobject<UBoxComponent>("Spawn Box");
	if (SpawnBoundingBox)
	{
		SpawnBoundingBox->SetCollisionProfileName("NoCollision");
		SetRootComponent(SpawnBoundingBox);
	}
}

// sets the spawner position, taking into account player pawn position
// adjusts its position
void ARadialActorsSpawner::SetSpawnerPosition()
{
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PawnLocation = PlayerPawn->GetActorLocation();
	if (PlayerPawn)
	{
		// take player position and place it higher a little bit
		PawnLocation.Z += SpawnRules.SpawnRadius;
		SetActorLocation(PawnLocation);
	}
}

// Called when the game starts or when spawned
void ARadialActorsSpawner::BeginPlay()
{
	Super::BeginPlay();

	// set the size of the bounding box
	SetSpawnerPosition();
	SpawnBoundingBox->SetBoxExtent(FVector(SpawnRules.SpawnRadius, SpawnRules.SpawnRadius, SpawnRules.SpawnRadius));
	SpawnTargetSpheres();
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
	FVector SpawnPointLocation;

	/*
		TO DO PICK OF BEST AVAILABLE POSITION, JUST IN CASE
	*/

	int32 attempts = 0;

	while (spawnedTargetsNb < SpawnRules.ActorsNb && attempts <= 100)
	{
		// get a random point in the bounding sphere and spawn an TargetSphere
		// we specify actor spawn parameters to nake it take into account collision with other objects on scene
		// Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found
		FActorSpawnParameters SpawnActorParameters;
		SpawnActorParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnPointLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), SpawnBoundingBox->Bounds.BoxExtent);
		ASphereTarget* CreatedTarget = GetWorld()->SpawnActor<ASphereTarget>(SpawnRules.SpawnObject, SpawnPointLocation, FRotator::ZeroRotator, SpawnActorParameters);
		if (CreatedTarget)
		{
			int32 attempts2 = 0;
			while (!isActorFarFromSpawnedActors(CreatedTarget) && attempts2 <= 100)
			{
				SpawnPointLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), SpawnBoundingBox->Bounds.BoxExtent);
				CreatedTarget->SetActorLocation(SpawnPointLocation);
				attempts2++;
			}

			// add created target to the array of targets
			spawnedTargetsNb++;
			TargetSpheres.Add(CreatedTarget);
		}
		attempts++;
	}
}

int32	ARadialActorsSpawner::GetNumberOfSpawnedActors() const
{
	return TargetSpheres.Num();
}

// checks if the distance from the spawned target and the existing one
// also checks the distance between the spawned target and the player pawn
bool ARadialActorsSpawner::isActorFarFromSpawnedActors(ASphereTarget* SpawnedTarget) const
{
	// get player pawn and check if it is valid
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerPawn)
	{
		return false;
	}

	// loop through all the target spheres and check the distance
	for (int i = 0; i < TargetSpheres.Num(); i++)
	{
		if (IsValid(TargetSpheres[i]))
		{
			float DistanceBetweenPoints = (SpawnedTarget->GetActorLocation() - TargetSpheres[i]->GetActorLocation()).Size();
			float DistanceBetweenPointAndPlayer = (PlayerPawn->GetActorLocation() - TargetSpheres[i]->GetActorLocation()).Size();
			if (DistanceBetweenPoints < SpawnRules.DistanceBetweenObjects || DistanceBetweenPointAndPlayer < SpawnRules.DistanceBetweenObjects)
			{
				//UE_LOG(LogTemp, Warning, TEXT("The targetr is too close %f or %f"), DistanceBetweenPoints, DistanceBetweenPointAndPlayer)
				return false;
			}
		}
	}

	return true;
}

// update the spawner parameters
// such as number of actors and spawn radius
void ARadialActorsSpawner::StartNewWave()
{
	// update number of actor on the certain percentage
	SpawnRules.ActorsNb += ((float)SpawnRules.ActorsNb * (SpawnRules.ActorsNbStep / 100.0f));
	// update spawnRadius of actor on the certain percentage and its box extent and its position
	SpawnRules.SpawnRadius += (SpawnRules.SpawnRadius * (SpawnRules.SpawnRadiusStep / 100.f));
	SpawnBoundingBox->SetBoxExtent(FVector(SpawnRules.SpawnRadius, SpawnRules.SpawnRadius, SpawnRules.SpawnRadius));
	SetSpawnerPosition();
	// remove all invalid objects from array
	UpdatedActorsArray();
	// spawn new objects
	SpawnTargetSpheres();

	UE_LOG(LogTemp, Warning, TEXT("SpawnRules.ActorsNb = %d, SpawnRules.SpawnRadius = %f, SpawnRules.ActorsNbStep = %f"), SpawnRules.ActorsNb, SpawnRules.SpawnRadius, SpawnRules.ActorsNbStep)
}

// updated the actors array, it removes add invalid objects
void	ARadialActorsSpawner::UpdatedActorsArray()
{
	TArray<int32>	IndexesToRemove;

	TargetSpheres.RemoveAll([](ASphereTarget* Val) {
		return !IsValid(Val);
		});
}



