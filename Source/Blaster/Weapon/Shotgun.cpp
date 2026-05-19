// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (InstigatorPawn == nullptr) return;
	
	AController* InstigatorController = InstigatorPawn->GetController();

	
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket )
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		
		TMap<ABlasterCharacter*,uint32> HitMap;
		for (uint32 i=0; i < NumberOfPellets;i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start,HitTarget,FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter && HasAuthority() && InstigatorController)
			{
				if (HitMap.Contains(BlasterCharacter))
				{
					HitMap[BlasterCharacter]++;
				}
				else
				{
					HitMap.Emplace(BlasterCharacter,1);
				}
				
			}
			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactEffect,FireHit.ImpactPoint,FireHit.ImpactNormal.Rotation());
			}
			if (ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,FireHit.ImpactPoint,0.25f,FMath::RandRange(-0.5f,0.5f));
			}
		}
		for ( auto HitPair : HitMap)
		{
			if (InstigatorController)
			{
				UGameplayStatics::ApplyDamage(HitPair.Key,Damage * HitPair.Value,InstigatorController,this,UDamageType::StaticClass());
			}
		}
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),MuzzleFlash,SocketTransform);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,FireSound,GetActorLocation());
		}
		
	}
}
