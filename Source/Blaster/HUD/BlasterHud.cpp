// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHud.h"

#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void ABlasterHud::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X/2.f, ViewportSize.Y/2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairsSpread;
		
		if(HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f,0.f);
			DrawCrosshairs(HUDPackage.CrosshairsCenter,ViewportCenter,Spread,HUDPackage.CrosshairsColor);
		}
		if(HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled,0.f);
			DrawCrosshairs(HUDPackage.CrosshairsLeft,ViewportCenter,Spread,HUDPackage.CrosshairsColor);
		}
		if(HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled,0.f);
			DrawCrosshairs(HUDPackage.CrosshairsRight,ViewportCenter,Spread,HUDPackage.CrosshairsColor);
		}
		if(HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f,-SpreadScaled);
			DrawCrosshairs(HUDPackage.CrosshairsTop,ViewportCenter,Spread,HUDPackage.CrosshairsColor);
		}
		if(HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f,SpreadScaled);
			DrawCrosshairs(HUDPackage.CrosshairsBottom,ViewportCenter, Spread,HUDPackage.CrosshairsColor);
		}
	}
}

void ABlasterHud::BeginPlay()
{
	Super::BeginPlay();
	AddCharacterOverlay();
}

void ABlasterHud::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHud::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D DrawPoint(ViewportCenter.X - (TextureWidth/2.f) + Spread.X, ViewportCenter.Y - (TextureHeight/2.f) + Spread.Y);

	DrawTexture(Texture,DrawPoint.X,DrawPoint.Y,TextureWidth,TextureHeight,0.f,0.f,1.f,1.f,CrosshairColor);
}
