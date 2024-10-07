// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	ShellEjectionImpulseStrength =10.f;
	Mesh->SetNotifyRigidBodyCollision(true);
}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this,&ACasing::ACasing::OnHit);
	Mesh->AddImpulse(GetActorForwardVector()*ShellEjectionImpulseStrength);
	
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
	if(CasingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,CasingSound,GetActorLocation());
	}
}



