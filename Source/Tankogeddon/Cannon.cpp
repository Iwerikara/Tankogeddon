// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/ArrowComponent.h>
#include <Engine/Engine.h>
#include <TimerManager.h>
#include <Engine/World.h>

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

	AltFireShotsLeft = AltFireShots;
}

void ACannon::Fire()
{
	if (!bReadyToFire)
	{
		return;
	}

	bReadyToFire = false;

	Shot();

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
}

void ACannon::AltFire()
{
	if (!bReadyToFire)
	{
		return;
	}

	bReadyToFire = false;

	AltShot();

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1.f / FireRate, false);
}

void ACannon::Shot()
{

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, "Fire - projectile");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, "Fire - trace");
	}
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
}