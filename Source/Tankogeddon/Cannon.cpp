// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include "DrawDebugHelpers.h"
#include "Tankogeddon.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "Camera/CameraShake.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/ArrowComponent.h>
#include <Engine/Engine.h>
#include <TimerManager.h>
#include <Engine/World.h>
#include <Particles/ParticleSystemComponent.h>
#include <Components/AudioComponent.h>

// Sets default values
ACannon::ACannon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	ProjectileSpawnPoint->SetupAttachment(Mesh);

	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Shoot effect"));
	ShootEffect->SetupAttachment(ProjectileSpawnPoint);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio effect"));
	AudioEffect->SetupAttachment(ProjectileSpawnPoint);

}

void ACannon::Fire()
{
		if (CurrentAmmo == 0)
		{
			return;
		}

	if (!bReadyToFire)
	{
		return;
	}
	bReadyToFire = false;

	Shot();

}

void ACannon::AltFire()
{
	if (CurrentAmmo == 0)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Orange, "No ammo!");
		return;
	}

	if (!bReadyToFire)
	{
		return;
	}

	bReadyToFire = false;

	AltShot();

	CurrentAmmo--;

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
}

void ACannon::Shot()
{

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, "Fire - projectile");

		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		if (Projectile)
		{
			Projectile->Start();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, "Fire - trace");

		FHitResult HitResult;
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = false;

		FVector Start = ProjectileSpawnPoint->GetComponentLocation();
		FVector End = ProjectileSpawnPoint->GetForwardVector() * FireRange + Start;
		DrawDebugPoint(GetWorld(), End, 20.f, FColor::Cyan, false, 0.2f);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, TraceParams))
		{
			DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Purple, false, 0.5f, 0, 5);

			IDamageTaker* DamageTakerActor = Cast<IDamageTaker>(HitResult.Actor.Get());

			if (DamageTakerActor)
			{
				FDamageData DamageData;
				DamageData.DamageValue = FireDamage;
				DamageData.Instigator = this;
				DamageData.DamageMaker = this;

				DamageTakerActor->TakeDamage(DamageData);
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Purple, false, 0.5f, 0, 5);
		}
	}

	ShootEffect->ActivateSystem();
	AudioEffect->Play();

	if (GetOwner() && GetOwner() == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		if (ShootForceEffect)
		{
			FForceFeedbackParameters ShootForceEffectParams;
			ShootForceEffectParams.bLooping = false;
			ShootForceEffectParams.Tag = "shootForceEffectParams";
			GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(ShootForceEffect, ShootForceEffectParams);
		}

		if (ShootShake)
		{
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(ShootShake);
		}
	}

	CurrentAmmo--;

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
}

void ACannon::AltShot()
{
	if (AltFireShotsLeft == 0)
	{
		AltFireShotsLeft = AltFireShots;
		return;
	}

	Shot();

	AltFireShotsLeft--;

	GetWorld()->GetTimerManager().SetTimer(AltFireTimerHandle, this, &ACannon::AltShot, AltFireShotDelay, false);
}

bool ACannon::IsReadyToFire()
{
	return bReadyToFire;
}

void ACannon::Reload()
{
	bReadyToFire = true;
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	Reload();
}

void ACannon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AltFireTimerHandle);

	int32 ACannon::GetMaxAmmo()
	{
		return MaxAmmo;
	}

	int32 ACannon::GetCurrentAmmo()
	{
		return CurrentAmmo;
	}

	void ACannon::SetCurrentAmmo(int32 Ammo)
	{
		CurrentAmmo = Ammo;
	}

	bool ACannon::AddAmmo(int32 Ammo)
	{
		if (CurrentAmmo == MaxAmmo)
		{
			return false;
		}

		if (CurrentAmmo + Ammo >= MaxAmmo)
		{
			CurrentAmmo = MaxAmmo;
		}
		else
		{
			CurrentAmmo += Ammo;
		}

		return true;
	}
}