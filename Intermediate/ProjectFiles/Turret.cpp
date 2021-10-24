// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "MalchitsVS_UEBasics.h"
#include "TankPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Cannon.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include <Particles/ParticleSystemComponent.h>
#include <Components/AudioComponent.h>

// Sets default values
ATurret::ATurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret turret"));
	TurretMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(TurretMesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddDynamic(this, &ATurret::Die);
	HealthComponent->OnDamaged.AddDynamic(this, &ATurret::DamageTaken);

	UStaticMesh* TurretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
	if (TurretMeshTemp)
	{
		TurretMesh->SetStaticMesh(TurretMeshTemp);
	}

	UStaticMesh* BodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
	if (BodyMeshTemp)
	{
		BodyMesh->SetStaticMesh(BodyMeshTemp);
	}

	DeathParticleEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Turret death particle effect"));
	DeathParticleEffect->SetupAttachment(RootComponent);

	DeathAudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Turret death audio effect"));
	DeathAudioEffect->SetupAttachment(RootComponent);

	HittedAudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Hitted audio effect"));
	HittedAudioEffect->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ACannon>(CannonClass, Params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	FTimerHandle TargetingTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TargetingTimerHandle, this, &ATurret::Targeting, TargetingRate, true, TargetingRate);
}

void ATurret::Destroyed()
{
	if (Cannon)
	{
		Cannon->Destroy();
	}
}

void ATurret::Targeting()
{
	if (IsPlayerInRange())
	{
		RotateToPlayer();

	if (CheckCanFire() && Cannon && Cannon->IsReadyToFire())
		{
			Fire();
		}
	}
}

void ATurret::RotateToPlayer()
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPawn->GetActorLocation());
	FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	TargetRotation.Pitch = CurrentRotation.Pitch;
	TargetRotation.Roll = CurrentRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), TargetingSpeed));
}

bool ATurret::IsPlayerInRange()
{
	return FVector::Distance(TargetPawn->GetActorLocation(), GetActorLocation()) <= TargetingRange;
}

bool ATurret::CheckCanFire()
{
	if (!CheckIsPlayerSeen())
	{
		return false;
	}

	FVector TargetingDirection = TurretMesh->GetForwardVector();
	FVector DirectionToPlayer = TargetPawn->GetActorLocation() - GetActorLocation();
	DirectionToPlayer.Normalize();
	float AimAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(TargetingDirection, DirectionToPlayer)));

	return AimAngle <= Accurency;
}

void ATurret::Fire()
{
	if (Cannon)
	{
		Cannon->Fire();
	}
}

void ATurret::TakeDamage(FDamageData DamageData)
{
	HittedAudioEffect->Play();
	HealthComponent->TakeDamage(DamageData);
}

void ATurret::Die()
{
	if (isDead)
	{
		return;
	}

	isDead = true;

	DeathParticleEffect->ActivateSystem();
	DeathAudioEffect->Play();

	TargetingSpeed = 0.f;
	TargetingRange = 0.f;

	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &ATurret::DestroyTurret, 2.f, false, 2.f);
}

void ATurret::DestroyTurret()
{
	Destroy();
}

void ATurret::DamageTaken(float DamageValue)
{
	UE_LOG(LogTankogeddon, Warning, TEXT("Turret %s taked damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}

bool ATurret::CheckIsPlayerSeen()
{
	FVector PlayerPos = TargetPawn->GetActorLocation();
	FVector EyesPos = CannonSetupPoint->GetComponentLocation();

	FHitResult HitResult;
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(Cannon);
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, EyesPos, PlayerPos, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		if (HitResult.Actor.Get())
		{
			DrawDebugLine(GetWorld(), EyesPos, HitResult.Location, FColor::Red, false, 0.2f, 0, 10);
			//UE_LOG(LogTankogeddon, Warning, TEXT("Hit result: %s"), *HitResult.Actor.Get()->GetName());

			return HitResult.Actor.Get() == TargetPawn;
		}
	}
	DrawDebugLine(GetWorld(), EyesPos, PlayerPos, FColor::Blue, false, 0.2f, 0, 10);

	return false;
}