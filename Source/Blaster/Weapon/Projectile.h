// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
    virtual void Tick(float DeltaTime) override;
protected:
	
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);
	
	private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	


private:
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent* TracerComponent;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactEffect;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	
public:
	FORCEINLINE float GetDamage() const { return Damage; }
};
