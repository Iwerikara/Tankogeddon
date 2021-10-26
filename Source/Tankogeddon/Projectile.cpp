// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Tankogeddon.h"
#include <Particles/ParticleSystemComponent.h>
#include <Components/AudioComponent.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceeneCpm = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceeneCpm;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnMeshOverlapBegin);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
}

void AProjectile::Start()
{
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AProjectile::Move, MoveRate, true, MoveRate);
}

void AProjectile::Stop()
{
	if (bExploding)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticleEffect, GetActorTransform());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodeAudioEffect, GetActorLocation());

		Explode();
	}
	Destroy();
}


void AProjectile::OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTankogeddon, Warning, TEXT("Projectile %s collided with %s. "), *GetName(), *OtherActor->GetName());

	AActor* OwnerActor = GetOwner();
	AActor* OwnerByOwner = OwnerActor != nullptr ? OwnerActor->GetOwner() : nullptr;
	if (OtherActor != OwnerActor && OtherActor != OwnerByOwner)
	{
		IDamageTaker* DamageTakerActor = Cast<IDamageTaker>(OtherActor);
		if (DamageTakerActor)
		{
			FDamageData DamageData;
			DamageData.DamageValue = Damage;
			DamageData.Instigator = OwnerActor;
			DamageData.DamageMaker = this;

			DamageTakerActor->TakeDamage(DamageData);
		}
		else
		{
			UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(OtherComp);
			if (PrimComponent && PrimComponent->IsSimulatingPhysics())
			{
				//PrimComponent->AddImpulseAtLocation(ForceVector * PushForce, SweepResult.ImpactPoint);
				PrimComponent->AddForceAtLocation(ForceVector * PushForce, SweepResult.ImpactPoint);
			}

			//OtherActor->Destroy();
		}

		Destroy();
		Stop();
	}
}

void AProjectile::Move()
{
	FVector NextPosition = GetActorLocation() + GetActorForwardVector() * MoveSpeed * MoveRate;
	SetActorLocation(NextPosition);
}

void AProjectile::Explode()
{
	FVector StartPos = GetActorLocation();
	FVector EndPos = StartPos + FVector(0.1f);

	FCollisionShape Shape = FCollisionShape::MakeSphere(ExplodeRadius);
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;
	Params.TraceTag = "Explode Trace";
	TArray<FHitResult> AttackHit;

	FQuat Rotation = FQuat::Identity;

	bool SweepResult = GetWorld()->SweepMultiByChannel
	(
		AttackHit,
		StartPos,
		EndPos,
		Rotation,
		ECollisionChannel::ECC_Visibility,
		Shape,
		Params
	);

	GetWorld()->DebugDrawTraceTag = "Explode Trace";

	if (SweepResult)
	{
		TArray<FHitResult*> Attacked;

		for (FHitResult HitResult : AttackHit)
		{
			if (Attacked.Find(&HitResult) != INDEX_NONE)
			{
				continue;
			}
			AActor* OtherActor = HitResult.GetActor();
			if (!OtherActor)
			{
				continue;
			}

			MakeDamageOrAddForce(OtherActor);

			Attacked.AddUnique(&HitResult);
		}
	}
}

void AProjectile::MakeDamageOrAddForce(AActor* OtherActor)
{
	IDamageTaker* DamageTakerActor = Cast<IDamageTaker>(OtherActor);
	if (DamageTakerActor)
	{
		FDamageData DamageData;
		DamageData.DamageValue = Damage;
		DamageData.Instigator = GetOwner();
		DamageData.DamageMaker = this;

		DamageTakerActor->TakeDamage(DamageData);
	}
	else
	{
		UPrimitiveComponent* OtherMesh = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
		if (OtherMesh)
		{
			if (OtherMesh->IsSimulatingPhysics())
			{
				FVector ForceVector = OtherActor->GetActorLocation() - GetActorLocation();
				ForceVector.Normalize();
				OtherMesh->AddImpulse(ForceVector * PushForce, NAME_None, true);
			}
		}
	}
}