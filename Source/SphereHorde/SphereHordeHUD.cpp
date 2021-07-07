// Copyright Epic Games, Inc. All Rights Reserved.

#include "SphereHordeHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "SphereHordeGameMode.h"
#include "UObject/ConstructorHelpers.h"

ASphereHordeHUD::ASphereHordeHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
	ScoreMessage = "Score";
	WaveMessage = "Wave";
}

void ASphereHordeHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	// draw the score and wave number to the HUD
	ASphereHordeGameMode* GameMode = Cast<ASphereHordeGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		FString  FinalScoreText = FString::Printf(TEXT("%s : %d %s : %d"), *ScoreMessage, GameMode->GetCurrentDestroyedSpheresNumber(), *WaveMessage, GameMode->GetCurrentWaveNumber());
		DrawText(FinalScoreText, FLinearColor::Black, 0, 0, Font, 1.5f, false);
	}
}
