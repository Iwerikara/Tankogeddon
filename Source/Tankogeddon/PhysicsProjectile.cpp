// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsProjectile.h"
#include "DrawDebugHelpers.h"
#include "PhysicsComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <particles/ParticleSystemComponent.h>

APhysicsProjectile::APhysicsProjectile()
{
	PhysicsComponent = CreateDefaultSubobject<UPhysicsComponent>(TEXT("PhysicsComponent"));

	TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail effect"));
	TrailEffect->SetupAttachment(RootComponent);
}

void APhysicsProjectile::Start()
{
	MoveVector = GetActorForwardVector() * TrajectorySimulationSpeed;
	CurrentTrajectory = PhysicsComponent->GenerateTrajectory(GetActorLocation(), MoveVector, TrajectorySimulationMaxTime, TrajectorySimulationTimeStep, 0);
	if (bShowTrajectory)
	{
		for (FVector Position : CurrentTrajectory)
		{
			DrawDebugSphere(GetWorld(), Position, 5, 8, FColor::Purple, true, 1, 0, 2);
		}
	}

	TragectoryPointIndex = 0;
	Super::Start();
}

void APhysicsProjectile::Move()
{
	FVector CurrentMoveVector = CurrentTrajectory[TragectoryPointIndex] - GetActorLocation();
	CurrentMoveVector.Normalize();
	FVector NewLocation = GetActorLocation() + CurrentMoveVector * MoveSpeed * MoveRate;
	SetActorLocation(NewLocation);
	if (FVector::Distance(NewLocation, CurrentTrajectory[TragectoryPointIndex]) <= MoveAccurency)
	{
		TragectoryPointIndex++;
		if (TragectoryPointIndex >= CurrentTrajectory.Num())
		{
			Explode();
			Destroy();
		}
		else
		{
			FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTrajectory[TragectoryPointIndex]);
			SetActorRotation(NewRotation);
		}
	}
}

void APhysicsProjectile::Explode()
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
		for (FHitResult HitResult : AttackHit)
		{
			AActor* OtherActor = HitResult.GetActor();
			if (!OtherActor)
			{
				continue;
			}

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
	}
}