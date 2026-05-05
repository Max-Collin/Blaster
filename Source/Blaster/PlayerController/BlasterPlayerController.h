// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHudHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats );
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeaponIcon(UTexture2D* Icon);
	void SetHUDMatchCountdown(float MatchCountdownTime);
	void ClearHUDIcon();
	virtual void OnPossess(APawn* PossessedPawn);
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;

	void SetHUDTime();
private:
	UPROPERTY()
	class ABlasterHud* BlasterHud;

	float MatchTime = 120.f;
	uint32 CountdownInt = 0;
	
	
};
