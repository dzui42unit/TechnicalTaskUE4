// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "SphereTarget.h"

// Sets default values
ASphereTarget::ASphereTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create a capsule component and set it a as a root component
	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CollisionCapsule->SetCapsuleSize(CollisionSphereRadius, CollisionSphereRadius / 2.f);
	SetRootComponent(CollisionCapsule);

	// create a mesh component and attach it to the capsule (root) component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	Mesh->SetupAttachment(CollisionCapsule);
}

void ASphereTarget::PlayDeathEffectsAndDestroy()
{
	// play destruction vfx if the DestructionParticle particle system is not nullptr
	if (DestructionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructionParticle, GetActorLocation());
		Destroy();
	}
}

// Called when the game starts or when spawned
void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASphereTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

