// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SphereHordeHUD.generated.h"

class UFont;

UCLASS()
class ASphereHordeHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASphereHordeHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Score")
	FString		ScoreMessage;

	UPROPERTY(EditDefaultsOnly, Category = "Score")
	FString		WaveMessage;

	UPROPERTY(EditDefaultsOnly, Category = "Score")
	UFont*		Font;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
};

