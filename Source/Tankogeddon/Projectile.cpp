// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

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

void AProjectile::OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			OtherActor->Destroy();
		}

		Destroy();
	}
}

void AProjectile::Move()
{
	FVector NextPosition = GetActorLocation() + GetActorForwardVector() * MoveSpeed * MoveRate;
	SetActorLocation(NextPosition);
}