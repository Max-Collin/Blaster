// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterHud.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHud = Cast<ABlasterHud>(GetHUD());
}

void ABlasterPlayerController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(PossessedPawn);
	if (BlasterCharacter)
	{
		SetHudHealth(BlasterCharacter->GetHealth(),BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::SetHudHealth(float Health, float MaxHealth)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->HealthBar && BlasterHud->CharacterOverlay->HealthText)
	{
		const float HealthPercentage = Health / MaxHealth;
		BlasterHud->CharacterOverlay->HealthBar->SetPercent(HealthPercentage);

		FString HealthText =  FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Health),FMath::CeilToInt(MaxHealth));
		BlasterHud->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->ScoreAmount )
	{
		FString ScoreText =  FString::Printf(TEXT("%d"),FMath::FloorToInt(Score));
		BlasterHud->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->DefeatsAmount )
	{
		FString DefeatsText =  FString::Printf(TEXT("%d"),Defeats);
		BlasterHud->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;
	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->WeaponAmmoAmount )
	{
		FString AmmoText =  FString::Printf(TEXT("%d"),Ammo);
		BlasterHud->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

