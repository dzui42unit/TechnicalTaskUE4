// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "RadialActorsSpawner.h"

// the constructor that takes number of actors and number of inner radius actors for creation of spawner object
ARadialActorsSpawner::ARadialActorsSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnRules.InnerSpawnRadius = 1500.f;
	SpawnRules.OutterSpawnRadius = 2000.f;
	SpawnRules.DistanceBetweenObjects = 80.f;

	// create a bounding box components, it defines the area where to pick a location for spawn,
	// set it as root component
	// the radius is defined by the SpawnRules
	OutterSpawnBoundingBox = CreateDefaultSubobject<UBoxComponent>("Outter Spawn Box");
	if (OutterSpawnBoundingBox)
	{
		OutterSpawnBoundingBox->SetCollisionProfileName("NoCollision");
		OutterSpawnBoundingBox->SetBoxExtent(FVector(0.f, 0.f, 0.f));
		SetRootComponent(OutterSpawnBoundingBox);
	}

	// create inner radius, to spawn objects to be near the spawn origin
	InnerSpawnBoundingBox = CreateDefaultSubobject<UBoxComponent>("Inner Spawn Box");
	if (InnerSpawnBoundingBox)
	{
		InnerSpawnBoundingBox->SetCollisionProfileName("NoCollision");
		SetRootComponent(InnerSpawnBoundingBox);
		InnerSpawnBoundingBox->SetBoxExtent(FVector(0.f, 0.f, 0.f));
		OutterSpawnBoundingBox->SetupAttachment(InnerSpawnBoundingBox);
	}
}

// the function to initialize number of actors to spawn
void ARadialActorsSpawner::Initialize(int32 ActorsNb, int32 InnerRadiusNb)
{
	if (ActorsNb < InnerRadiusNb)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActorsNb < InnerRadiusNb, please check the values!"))
		SpawnRules.ActorsNb = 15;
		SpawnRules.InnerRadiusActorsNb = 10;
		return;
	}

	SpawnRules.ActorsNb = ActorsNb;
	SpawnRules.InnerRadiusActorsNb = InnerRadiusNb;
}

// sets the spawner position, taking into account player pawn position
// adjusts its position
void ARadialActorsSpawner::SetSpawnerPosition()
{
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PawnLocation = PlayerPawn->GetActorLocation();
	if (PlayerPawn)
	{
		// SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));

		// take player position and place it higher a little bit
		PawnLocation.Z += SpawnRules.OutterSpawnRadius;
		SetActorLocation(PawnLocation);
	}
}

// Called when the game starts or when spawned
void ARadialActorsSpawner::BeginPlay()
{
	Super::BeginPlay();

	// set inner and outter box extents
	OutterSpawnBoundingBox->SetBoxExtent(FVector(SpawnRules.OutterSpawnRadius, SpawnRules.OutterSpawnRadius, SpawnRules.OutterSpawnRadius));
	InnerSpawnBoundingBox->SetBoxExtent(FVector(SpawnRules.InnerSpawnRadius, SpawnRules.InnerSpawnRadius, SpawnRules.InnerSpawnRadius));
	// set the size of the bounding box
	SetSpawnerPosition();
	// spawn new objects in inner radius
	SpawnTargetSpheres(SpawnRules.InnerRadiusActorsNb, InnerSpawnBoundingBox->Bounds.BoxExtent, SpawnRules.InnerSpawnRadius);
	// spawn new objects in outter radius
	SpawnTargetSpheres(SpawnRules.ActorsNb - SpawnRules.InnerRadiusActorsNb, OutterSpawnBoundingBox->Bounds.BoxExtent, SpawnRules.OutterSpawnRadius);
}

// Called every frame
void ARadialActorsSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// spawns a N number of target spheres
void ARadialActorsSpawner::SpawnTargetSpheres(int32 NbOfSpheres, FVector BoxExtent, float Radius)
{
	if (!SpawnRules.SpawnObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnRules.SpawnObject is NOT set"))
		return;
	}

	// run the loop until needed number of the targets will not be created
	int32 spawnedTargetsNb = 0;
	FVector SpawnPointLocation;

	// spawn attempts to create a new wave a targets
	int32 SpawnAttempts = 0;
	while (spawnedTargetsNb < NbOfSpheres && SpawnAttempts < 1000)
	{
		SpawnAttempts++;

		// get a random point in the bounding sphere and spawn an TargetSphere
		// we specify actor spawn parameters to nake it take into account collision with other objects on scene
		// Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found
		FActorSpawnParameters SpawnActorParameters;
		SpawnActorParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnPointLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), BoxExtent);
		ASphereTarget* CreatedTarget = GetWorld()->SpawnActor<ASphereTarget>(SpawnRules.SpawnObject, SpawnPointLocation, FRotator::ZeroRotator, SpawnActorParameters);
		if (CreatedTarget)
		{
			int32 AttemptsToFindPosition = 0;
			while (!isActorFarFromSpawnedActors(CreatedTarget, Radius) && AttemptsToFindPosition < 1000)
			{
				SpawnPointLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), BoxExtent);
				CreatedTarget->SetActorLocation(SpawnPointLocation);
				AttemptsToFindPosition++;
			}

			if (AttemptsToFindPosition == 1000)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find proper position :("))
				continue;
			}

			// add created target to the array of targets
			spawnedTargetsNb++;
			TargetSpheres.Add(CreatedTarget);
		}
	}



	// remove all invalid objects from array
	UpdatedActorsArray();
}

int32	ARadialActorsSpawner::GetNumberOfSpawnedActors() const
{
	return TargetSpheres.Num();
}

// checks if the distance from the spawned target and the existing one
// also checks the distance between the spawned target and the player pawn
// and checks the actor position being in reachable distance from the box radius
bool ARadialActorsSpawner::isActorFarFromSpawnedActors(ASphereTarget* SpawnedTarget, float Radius) const
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
			float DistanceBetweenActorAndOrigin = (SpawnedTarget->GetActorLocation() - GetActorLocation()).Size();

			if (DistanceBetweenPoints < SpawnRules.DistanceBetweenObjects)
			{
				UE_LOG(LogTemp, Warning, TEXT("The target is too close to the the other actor: %f"), DistanceBetweenPoints)
				return false;
			}

			if (DistanceBetweenPointAndPlayer < SpawnRules.DistanceBetweenObjects)
			{
				UE_LOG(LogTemp, Warning, TEXT("The target is too close to the the player: %f"), DistanceBetweenPointAndPlayer)
				return false;
			}

			if (DistanceBetweenActorAndOrigin > Radius)
			{
				UE_LOG(LogTemp, Warning, TEXT("The target is too far form origin: %f"), DistanceBetweenActorAndOrigin)
				return false;
			}

			//if (DistanceBetweenPoints < SpawnRules.DistanceBetweenObjects
			//	|| DistanceBetweenPointAndPlayer < SpawnRules.DistanceBetweenObjects
			//		|| DistanceBetweenActorAndOrigin > Radius)
			//{
			//	UE_LOG(LogTemp, Warning, TEXT("The targetr is too close %f or %f or %f"), DistanceBetweenPoints, DistanceBetweenPointAndPlayer, DistanceBetweenActorAndOrigin)
			//	return false;
			//}
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
	SpawnRules.OutterSpawnRadius += (SpawnRules.OutterSpawnRadius * (SpawnRules.SpawnRadiusStep / 100.f));
	OutterSpawnBoundingBox->SetBoxExtent(FVector(SpawnRules.OutterSpawnRadius, SpawnRules.OutterSpawnRadius, SpawnRules.OutterSpawnRadius));
	SetSpawnerPosition();
	// spawn new objects in inner radius
	SpawnTargetSpheres(SpawnRules.InnerRadiusActorsNb, InnerSpawnBoundingBox->Bounds.BoxExtent, SpawnRules.InnerSpawnRadius);
	// spawn new objects in outter radius
	SpawnTargetSpheres(SpawnRules.ActorsNb - SpawnRules.InnerRadiusActorsNb, OutterSpawnBoundingBox->Bounds.BoxExtent, SpawnRules.OutterSpawnRadius);

	UE_LOG(LogTemp, Warning, TEXT("SpawnRules.ActorsNb = %d, SpawnRules.SpawnRadius = %f, SpawnRules.ActorsNbStep = %f"), SpawnRules.ActorsNb, SpawnRules.OutterSpawnRadius, SpawnRules.ActorsNbStep)
}

// updated the actors array, it removes add invalid objects
void	ARadialActorsSpawner::UpdatedActorsArray()
{
	TargetSpheres.RemoveAll([](ASphereTarget* Val) {
		return !IsValid(Val);
		});
}



