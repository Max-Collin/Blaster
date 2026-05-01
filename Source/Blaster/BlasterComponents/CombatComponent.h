// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/HUD/BlasterHud.h"
#include "Components/ActorComponent.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	void EquipWeapon(class AWeapon* WeaponToEquip);
	void Reload();
	UFUNCTION(blueprintCallable, Category = "CombatComponent")
	void FinishReloading();
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	
	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);
	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(server, Reliable)
	void ServerReload();

	void HandleReload();
	
private:
	UPROPERTY()
	ABlasterCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* PlayerController;
	UPROPERTY()
	class ABlasterHud* HUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon , VisibleAnywhere)
	AWeapon* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bAiming;
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	/*
	 * HUD and Crosshairs
	 */
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FVector HitTarget;
	FHUDPackage FhudPackage;

	/*
	 *Aiming and FOV
	 */
	// FOV when not aiming, set to camera's base FOV in beginplay
	float DefaultFOV;
	float CurrentFOV;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV =30.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed;

	

	void InterpFOV(float DeltaTime);

	/*
	 *Automatic Fire
	 */

	FTimerHandle FireTimer;

	bool bCanFire = true;
	
	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	//carried weapon for the current weapon
	UPROPERTY(replicated = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	void OnRep_CarriedAmmo();

	TMap<EWeaponTypes, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState )
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	
public:	
	
	 
		
};
