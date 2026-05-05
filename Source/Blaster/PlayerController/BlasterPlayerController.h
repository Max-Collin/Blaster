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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Sync with server world clock
	virtual float GetServerTime();
	// Sync with Server clock ASAP
	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);
protected:
	virtual void BeginPlay() override;
	void CheckTimeSync(float DeltaSeconds);

	void SetHUDTime();

	void PollInit();

	/*
	 * Sync time between client adn server
	 */
	
	// Request current server time passing in clients time when requested
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	
	// Reports the current server time client in response to ServerRequestServerTime
	UFUNCTION(Client,Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	// Difference between client and server time
	float ClientServerDelta = 0;

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
private:
	UPROPERTY()
	class ABlasterHud* BlasterHud;

	float MatchTime = 120.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing= OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();
	
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	float HUDDefeats;
};
