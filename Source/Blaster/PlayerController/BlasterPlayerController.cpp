// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHud.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHud = Cast<ABlasterHud>(GetHUD());
}
void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
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

void ABlasterPlayerController::SetHudScore(float Score)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->ScoreAmount )
	{
		FString ScoreText = FString::Printf(TEXT("%d"),FMath::FloorToInt(Score));
		BlasterHud->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHudDefeats(int32 Defeats)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->DefeatsAmount )
	{
		FString DefeatsText = FString::Printf(TEXT("%d"),Defeats);
		BlasterHud->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void ABlasterPlayerController::SetHudWeaponAmmo(int32 WeaponAmmo)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->WeaponAmmoAmount )
	{
		FString AmmoText = FString::Printf(TEXT("%d"),WeaponAmmo);
		BlasterHud->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHudCarriedAmmo(int32 CarriedAmmo)
{
	BlasterHud = BlasterHud == nullptr ? Cast<ABlasterHud>(GetHUD()) : BlasterHud;

	if(BlasterHud && BlasterHud->CharacterOverlay && BlasterHud->CharacterOverlay->CarriedAmmoAmount )
	{
		FString AmmoText = FString::Printf(TEXT("%d"),CarriedAmmo);
		BlasterHud->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}
