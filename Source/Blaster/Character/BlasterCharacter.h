// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterComponents/CombatComponent.h"

#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"

#include "BlasterCharacter.generated.h"




class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()
	
		
	/*MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
    
	/*Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
    
	/*Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
    
	/*Look Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/*Equip Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	/*Crouch Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	/*Aim Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	/*Fire Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	/*Reload Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;
public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	


	virtual void OnRep_ReplicatedMovement();
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*Called for movement input */
	void Move(const FInputActionValue& Value);

	 /*Called for looking input */
	void Look(const FInputActionValue& Value);

	void Equip();
	void Reload();
	void Crouching();
	virtual void Jump() override;
	void AimButtonPressed();
	void AimButtonReleased();
	void CalculateAO_Pitch();
	float CalculateSpeed();
	void AimOffset(float DeltaTime);
	void SimProxiesTurn();
	
	void FireButtonPressed();
	void FireButtonReleased();

	void PlayHitReactMontage();
	void PlayElimMontage();
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor,float Damage, const UDamageType* DamageType, class AController* InstigatedController,AActor* DamageCauser);
	void UpdateHUDHeath();

	//Poll for any relevant classes and initialize the HUD
	void PollInit();
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess))
	class UWidgetComponent* OverheadWidget;
	
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	void TurnInPlace(float DeltaTime);
	ETurningInPlace TurningInPlace;
	UPROPERTY(EditAnywhere,Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere,Category = Combat)
	 UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere,Category = Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere,Category = Combat)
	UAnimMontage* ReloadMontage;
	
	void HideCameraIfCharacterClose();
	UPROPERTY(EditAnywhere)
	float CameraThreshold= 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastReplicationMovement;

	/*
	 *Player Health
	 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_Health,VisibleAnywhere,Category = "Player Stats" )
	float Health = MaxHealth;
	
	UFUNCTION()
	void OnRep_Health();
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bElim = false;

	FTimerHandle ElimTimerHandle;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	void ElimTimerFinished();

	/*
	 *Dissolve Effect
	 */
	UPROPERTY(visibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	
	FOnTimelineFloat DissolveTrack;
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;
	// Dynamic instance that can be changed at runtime
	UPROPERTY(visibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	// Material instance set on the blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;
	
	/*
	* Elim Bot
	 */
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;
	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;


	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;
	
	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return  TurningInPlace;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}
	FORCEINLINE bool ShouldRotateRootBone() const {return bRotateRootBone;}
	FORCEINLINE bool IsElim() const {return bElim;}
	FORCEINLINE float GetHealth() const {return Health;}
	FORCEINLINE float GetMaxHealth() const {return MaxHealth;}
	ECombatState GetCombatState() const ;

	
};
