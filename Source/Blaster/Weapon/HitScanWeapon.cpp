// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (InstigatorPawn == nullptr) return;
	
	AController* InstigatorController = InstigatorPawn->GetController();

	
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket )
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget,FireHit);
		UWorld* World = GetWorld();
		
		if (FireHit.bBlockingHit)
		{
			
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter && HasAuthority() && InstigatorController)
			{
				UGameplayStatics::ApplyDamage(BlasterCharacter,Damage,InstigatorController,this,UDamageType::StaticClass());
			}
			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World,ImpactEffect,FireHit.ImpactPoint,FireHit.ImpactNormal.Rotation());
			}
			if (ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());
			}
		}
		
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World,MuzzleFlash,SocketTransform);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,FireSound,GetActorLocation());
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget- TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f,SphereRadius);
	FVector EndLoc = SphereCenter + RandVector;
	FVector ToEndLoc = EndLoc - TraceStart;
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(),SphereCenter,SphereRadius,12,FColor::Red,false,5.f);
		DrawDebugSphere(GetWorld(),EndLoc,4.f,12,FColor::Orange,false,5.f);
		DrawDebugLine(GetWorld(),TraceStart,FVector(TraceStart+ToEndLoc*TRACE_LENGTH / ToEndLoc.Size()),FColor::Cyan,false,5.f);
	}

	return FVector(TraceStart+ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart , const FVector& HitTarget, FHitResult& OutHit)
{
	
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = bUseScatter? TraceEndWithScatter(TraceStart,HitTarget): TraceStart +(HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(OutHit,TraceStart,End,ECC_Visibility);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World,BeamParticles,TraceStart);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"),BeamEnd);
			}
		}
	}
}
