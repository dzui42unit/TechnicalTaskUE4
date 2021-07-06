// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SphereTarget.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;

UCLASS()
class SPHEREHORDE_API ASphereTarget : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ASphereTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// static mesh component
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	// capsule component
	UCapsuleComponent* CollisionCapsule;

	UPROPERTY(EditDefaultsOnly)
	// capsule radius, 50.f by default
	float CollisionSphereRadius = 50.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
