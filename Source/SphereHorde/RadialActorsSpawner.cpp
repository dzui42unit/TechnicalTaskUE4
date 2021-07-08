// Fill out your copyright notice in the Description page of Project Settings.

#include "RadialActorsSpawner.h"
#include "SphereTarget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/BrushComponent.h"

// the constructor that takes number of actors and number of inner radius actors for creation of spawner object
ARadialActorsSpawner::ARadialActorsSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// the the maximum scale of the spawned actor
	MaxActorScale = 1.f;
	CurrentActorScale = MaxActorScale;

	// set spawner z offset to 0.f by default
	zOffset = 0.f;

	// create a bounding box components, it defines the area where to pick a location for spawn,
	// set it as root component, the radius is defined by the SpawnRules
	OutterSpawnBoundingBox = CreateDefaultSubobject<UBoxComponent>("Outter Spawn Box");
	
	if (!ensure(OutterSpawnBoundingBox != nullptr))
	{
		return;
	}
	OutterSpawnBoundingBox->SetCollisionProfileName("NoCollision");
	OutterSpawnBoundingBox->SetBoxExtent(FVector(0.f));
	SetRootComponent(OutterSpawnBoundingBox);

	// create inner radius, to spawn objects to be near the spawn origin
	InnerSpawnBoundingBox = CreateDefaultSubobject<UBoxComponent>("Inner Spawn Box");
	if (!ensure(InnerSpawnBoundingBox != nullptr))
	{
		return;
	}

	InnerSpawnBoundingBox->SetCollisionProfileName("NoCollision");
	InnerSpawnBoundingBox->SetBoxExtent(FVector(0.f));
	// disable inner radius change its scale
	InnerSpawnBoundingBox->SetAbsolute(false, false, false);
	InnerSpawnBoundingBox->SetupAttachment(OutterSpawnBoundingBox);

}

// the function to initialize number of actors to spawn
void ARadialActorsSpawner::Initialize(int32 ActorsNb, int32 InnerRadiusNb)
{
	// check if number actors to be spawned in the inner radius (10 by default) is less thanthe total number of actors
	if (ActorsNb < InnerRadiusNb)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActorsNb < InnerRadiusNb, please check the values!"))
		SpawnRules.ActorsNb = 15;
		SpawnRules.InnerRadiusActorsNb = 10;
		return;
	}

	// set the values
	SpawnRules.ActorsNb = ActorsNb;
	SpawnRules.InnerRadiusActorsNb = InnerRadiusNb;
}

// Called when the game starts or when spawned
void ARadialActorsSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!OutterSpawnBoundingBox || !InnerSpawnBoundingBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inner and outter box"))
		return;
	}

	// update offset and box height
	UpdateZoffsetAndBoxHeight();

	// set inner and outter box extents
	OutterSpawnBoundingBox->SetBoxExtent(BoxExtentOutter);
	InnerSpawnBoundingBox->SetBoxExtent(BoxExtentInner);

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

// sets the spawner position, taking into account player pawn position
// adjusts its position
void	ARadialActorsSpawner::SetSpawnerPosition()
{
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PawnLocation = PlayerPawn->GetActorLocation();
	if (PlayerPawn)
	{
		// take player position and place spawner and adjust its position
		PawnLocation.Z += zOffset;
		SetActorLocation(PawnLocation);
	}
}

// get all sphere target actors on level
TArray<AActor*>	ARadialActorsSpawner::GetTargetsOnLevel() const
{

	TArray<AActor*>	ResultingArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASphereTarget::StaticClass(),ResultingArray);

	return ResultingArray;
}

// updates the box extent of the inner and outter box
void	ARadialActorsSpawner::UpdateZoffsetAndBoxHeight()
{
	// set the box extent values by default to the corresponding radiuses
	BoxExtentInner = FVector(SpawnRules.InnerSpawnRadius);
	BoxExtentOutter = FVector(SpawnRules.OutterSpawnRadius);

	// make the height of the spawn boxes half size smaller
	if (!SpawnRules.SpawnObjectsUnderPawn)
	{
		BoxExtentInner.Z /= 2.f;
		BoxExtentOutter.Z /= 2.f;
		zOffset = BoxExtentOutter.Z;
		InnerSpawnBoundingBox->SetRelativeLocation(FVector(0.f, 0.f, BoxExtentInner.Z - BoxExtentOutter.Z));
	}
}

// spawns a N number of target spheres
void	ARadialActorsSpawner::SpawnTargetSpheres(int32 NbOfSpheres, const FVector& BoxExtent, float Radius)
{
	// check if SpawnObject is defined
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
	int32 AttemptsNumber = 1000;

	while (spawnedTargetsNb < NbOfSpheres && SpawnAttempts < AttemptsNumber)
	{
		SpawnAttempts++;

		// get a random point in the bounding sphere and spawn an TargetSphere
		// we specify actor spawn parameters to nake it take into account collision with other objects on scene
		// Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found
		FActorSpawnParameters SpawnActorParameters;
		SpawnActorParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// get the random point for spawning in the boxExtent
		// or random reachable point in radius
		SpawnPointLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), BoxExtent);

		// spawn an actor
		ASphereTarget* CreatedTarget = GetWorld()->SpawnActor<ASphereTarget>(SpawnRules.SpawnObject, SpawnPointLocation, FRotator::ZeroRotator, SpawnActorParameters);
		
		if (spawnedTargetsNb != 0)
		{
			// calculate and set new scale, skip first spawned item
			CurrentActorScale = FMath::Clamp((CurrentActorScale - SpawnRules.ScaleActorStep), SpawnRules.MinActorScale, 1.f);
			CreatedTarget->SetActorScale3D(FVector(CurrentActorScale));
		}

		if (CreatedTarget)
		{
			int32 AttemptsToFindPosition = 0;
			while (!isActorFarFromSpawnedActors(CreatedTarget, Radius) && AttemptsToFindPosition < AttemptsNumber)
			{ 
				SpawnPointLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), BoxExtent);
				CreatedTarget->SetActorLocation(SpawnPointLocation);
				AttemptsToFindPosition++;
			}

			if (AttemptsToFindPosition == 1000)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find proper position :("))
				CreatedTarget->Destroy();
				continue;
			}

			// increase counter of created targets
			spawnedTargetsNb++;
		}
	}
}

// checks if the distance from the spawned target and the existing one
// also checks the distance between the spawned target and the player pawn
// and checks the actor position being in reachable distance from the box radius
bool	ARadialActorsSpawner::isActorFarFromSpawnedActors(ASphereTarget* SpawnedTarget, float Radius) const
{
	// get player pawn and check if it is valid
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerPawn)
	{
		return false;
	}

	TArray<AActor*>	TargetSpheres = GetTargetsOnLevel();
	// loop through all the target spheres and check the distance
	for (int i = 0; i < TargetSpheres.Num(); i++)
	{
		if (IsValid(TargetSpheres[i]) && SpawnedTarget != TargetSpheres[i])
		{
			// calculate the distance between new spawned target and all existing targets
			// between player and new spawned target
			// between new spawned target and origin
			float DistanceBetweenPoints = (SpawnedTarget->GetActorLocation() - TargetSpheres[i]->GetActorLocation()).Size();
			float DistanceBetweenPointAndPlayer = (SpawnedTarget->GetActorLocation() - PlayerPawn->GetActorLocation()).Size();
			float DistanceBetweenActorAndOrigin = (SpawnedTarget->GetActorLocation() - GetActorLocation()).Size();

			if (DistanceBetweenPoints <= SpawnRules.DistanceBetweenObjects)
			{
				return false;
			}
			if (DistanceBetweenPointAndPlayer <= SpawnRules.DistanceBetweenObjects)
			{
				return false;
			}
			if (DistanceBetweenActorAndOrigin >= Radius)
			{
				return false;
			}
		}
	}

	return true;
}

// update the spawner parameters
// such as number of actors and spawn radius
void	ARadialActorsSpawner::StartNewWave()
{
	// reset the actor scale
	CurrentActorScale = MaxActorScale;
	// update number of actor on the certain percentage
	SpawnRules.ActorsNb += ((float)SpawnRules.ActorsNb * (SpawnRules.ActorsNbStep / 100.0f));
	// update spawnRadius of actor on the certain percentage and its box extent and its position
	SpawnRules.OutterSpawnRadius += (SpawnRules.OutterSpawnRadius * (SpawnRules.SpawnRadiusStep / 100.f));
	UpdateZoffsetAndBoxHeight();
	OutterSpawnBoundingBox->SetBoxExtent(BoxExtentOutter);
	// update offset and set new spawner position
	SetSpawnerPosition();
	// spawn new objects in inner radius
	SpawnTargetSpheres(SpawnRules.InnerRadiusActorsNb, InnerSpawnBoundingBox->Bounds.BoxExtent, SpawnRules.InnerSpawnRadius);
	// spawn new objects in outter radius
	SpawnTargetSpheres(SpawnRules.ActorsNb - SpawnRules.InnerRadiusActorsNb, OutterSpawnBoundingBox->Bounds.BoxExtent, SpawnRules.OutterSpawnRadius);
}

