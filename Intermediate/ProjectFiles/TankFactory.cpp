// Fill out your copyright notice in the Description page of Project Settings.


#include "TankFactory.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tankogeddon.h"
#include <Particles/ParticleSystemComponent.h>
#include <Components/AudioComponent.h>
#include <Kismet/GameplayStatics.h>

ATankFactory::ATankFactory()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComp;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
	BuildingMesh->SetupAttachment(SceneComp);

	TankSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	TankSpawnPoint->AttachToComponent(SceneComp, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(SceneComp);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddDynamic(this, &ATankFactory::Die);
	HealthComponent->OnDamaged.AddDynamic(this, &ATankFactory::DamageTaked);
}

void ATankFactory::BeginPlay()
{
	Super::BeginPlay();

	if (LinkedMapLoader)
	{
		LinkedMapLoader->SetIsActivated(false);
	}

	FTimerHandle TargetingTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TargetingTimerHandle, this, &ATankFactory::SpawnNewTank, SpawnTankRate, true, SpawnTankRate);
}

void ATankFactory::TakeDamage(FDamageData DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}

void ATankFactory::Die()
{
	if (LinkedMapLoader)
	{
		LinkedMapLoader->SetIsActivated(true);
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticleEffect, GetActorTransform());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathAudioEffect, GetActorLocation());

	Destroy();
}

void ATankFactory::DamageTaked(float DamageValue)
{
	UE_LOG(LogTankogeddon, Warning, TEXT("Factory %s taked damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}

void ATankFactory::SpawnNewTank()
{
	FTransform SpawnTransform(TankSpawnPoint->GetComponentRotation(), TankSpawnPoint->GetComponentLocation(), FVector(1.f));
	ATankPawn* NewTank = GetWorld()->SpawnActorDeferred<ATankPawn>(SpawnTankClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	NewTank->SetPatrollingPoints(TankWayPoints);
	UGameplayStatics::FinishSpawningActor(NewTank, SpawnTransform);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnTankAudioEffect, GetActorLocation());
}