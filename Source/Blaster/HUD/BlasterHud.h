// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHud.generated.h"

USTRUCT()
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairsSpread;
	FLinearColor CrosshairsColor =  FLinearColor::White;
};


/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHud : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	class UCharacterOverlay* CharacterOverlay;
	protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();
private:
	FHUDPackage HUDPackage;
	void DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread, FLinearColor CrosshairColor = FLinearColor::White);
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
