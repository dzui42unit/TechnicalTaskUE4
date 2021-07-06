// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SphereHordeHUD.generated.h"

UCLASS()
class ASphereHordeHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASphereHordeHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

