// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include <filesystem>

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHud.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Sound/SoundCue.h"


#define TRACE_LENGTH 80000.f
UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent,EquippedWeapon);
	DOREPLIFETIME(UCombatComponent,bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent,CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent,CombatState);

	
	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if(Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}

		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
	
	
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
	if(Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;
		
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
	
}
void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(Character == nullptr || Character->Controller == nullptr) return;

	PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : PlayerController;
	if(PlayerController )
	{
		HUD = HUD == nullptr ? Cast<ABlasterHud>(PlayerController->GetHUD()) : HUD;
		if(HUD)
		{
			
			if(EquippedWeapon)
			{
				
				FhudPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				FhudPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				FhudPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				FhudPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				FhudPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				
			}
			else
				{
				FhudPackage.CrosshairsCenter = nullptr;
				FhudPackage.CrosshairsTop = nullptr;
				FhudPackage.CrosshairsBottom = nullptr;
				FhudPackage.CrosshairsLeft = nullptr;
				FhudPackage.CrosshairsRight = nullptr;
				
			}
			//calculate crosshair spread

			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f,1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange,VelocityMultiplierRange,Velocity.Size());

			if(Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,2.25f,DeltaTime,2.25f);
			}
			else
			{
				CrosshairInAirFactor =FMath::FInterpTo(CrosshairInAirFactor,0.0f,DeltaTime,30.f);
			}
			if(bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,0.58f,DeltaTime,30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,0.f,DeltaTime,30.f);
			}
			CrosshairShootingFactor= FMath::FInterpTo(CrosshairShootingFactor,0.f,DeltaTime,40.f);
			FhudPackage.CrosshairsSpread = 0.25f + CrosshairVelocityFactor+ CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
			
			HUD->SetHUDPackage(FhudPackage);
			
			
		}
	}
}


void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(EquippedWeapon == nullptr) return;

	if(bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,EquippedWeapon->GetZoomedFOV(),DeltaTime,EquippedWeapon->GetZoomedInterpSpeed());
		
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,DefaultFOV,DeltaTime,ZoomInterpSpeed);
	}
	if(Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::StartFireTimer()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(FireTimer,this,&UCombatComponent::FireTimerFinished,EquippedWeapon->FireDelay);
}

void UCombatComponent::FireTimerFinished()
{
	bCanFire= true;
	if(EquippedWeapon == nullptr) return;
	
	if(bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}



void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}
void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming= bIsAiming;
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	return  !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatStates::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, StartSMGAmmo);
}


void UCombatComponent::Fire()
{
	if(CanFire())
	{
		bCanFire=false;
		ServerFire(HitTarget);
		
			CrosshairShootingFactor += 0.2f;
		
		StartFireTimer();
	}
	
}

void UCombatComponent::FireButtonPressed(bool bPressed) // called locally 
{
	
	bFireButtonPressed = bPressed;
	if(bFireButtonPressed && EquippedWeapon)
	{
		Fire();
	}
}



void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}
void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(EquippedWeapon==nullptr) return;
	if(Character && CombatState == ECombatStates::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2d ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		
	}
	FVector2d CrosshairLocation(ViewportSize.X/2, ViewportSize.Y/2);
	FVector CrosshairWorldPos;
	FVector CrosshairWorldDir;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this,0),
		CrosshairLocation,CrosshairWorldPos,
		CrosshairWorldDir
		);
	if(bScreenToWorld)
	{
		FVector Start = CrosshairWorldPos;
		if(Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation()-Start).Size();
			Start += CrosshairWorldDir*(DistanceToCharacter + 50.f);
		}
		
		
		FVector End = Start + CrosshairWorldDir * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult,Start,End,ECC_Visibility);

		if(TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			FhudPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			FhudPackage.CrosshairsColor = FLinearColor::White;
		}




		

		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			
		}
		else
		{
			
			/*DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
		);*/
		}
	}
}







void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
	EquippedWeapon = WeaponToEquip;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();
	
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUDCarriedAmmo(CarriedAmmo);

		if (EquippedWeapon->WeaponIconTexture)
		{
			PlayerController->SetHUDWeaponIcon(EquippedWeapon->WeaponIconTexture);
		}
	}
	

	if(EquippedWeapon->EquipSounds)
	{
		UGameplayStatics::PlaySoundAtLocation(this,EquippedWeapon->EquipSounds,Character->GetActorLocation());
	}
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
	Character->GetCharacterMovement()->bOrientRotationToMovement=false;
	Character->bUseControllerRotationYaw = true;
}



void UCombatComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if(HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}

		if(EquippedWeapon->EquipSounds)
		{
			UGameplayStatics::PlaySoundAtLocation(this,EquippedWeapon->EquipSounds,Character->GetActorLocation());
		}
		PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : PlayerController;
		if (PlayerController)
		{
			if (EquippedWeapon->WeaponIconTexture)
			{
				PlayerController->SetHUDWeaponIcon(EquippedWeapon->WeaponIconTexture);
			}
		}
		
		Character->GetCharacterMovement()->bOrientRotationToMovement=false;
		Character->bUseControllerRotationYaw = true;
	}
}
void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != ECombatStates::ECS_Reloading)
	{
		ServerReload();
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	
	if (Character->HasAuthority())
	{
		CombatState = ECombatStates::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr ) return;
	int32 ReloadAmount = AmountToReload();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo =  CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr ) return;

	
	CombatState = ECombatStates::ECS_Reloading;
	HandleReload();
	
}
void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag,AmountCarried);
		return FMath::Clamp(RoomInMag,0,Least);
		
	}

	return 0;
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatStates::ECS_Reloading:
		HandleReload();
		break;
	case ECombatStates::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatStates::ECS_MAX:
		break;
	}
}

