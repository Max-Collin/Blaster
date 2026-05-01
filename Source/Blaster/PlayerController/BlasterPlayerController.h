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
	void SetHudScore(float Score);
	void SetHudDefeats(int32 Defeats);
	void SetHudWeaponAmmo(int32 WeaponAmmo);
	void SetHudCarriedAmmo(int32 CarriedAmmo);
	virtual void OnPossess(APawn* InPawn) override;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	class ABlasterHud* BlasterHud;
	
	
};
