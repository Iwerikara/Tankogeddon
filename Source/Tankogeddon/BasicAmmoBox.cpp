// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicAmmoBox.h"
#include "TankPawn.h"
#include "Tankogeddon.h"

// Sets default values
ABasicAmmoBox::ABasicAmmoBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceeneCopm = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceeneCopm;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ABasicAmmoBox::OnMeshOverlapBegin);
	Mesh->SetCollisionProfileName(FName("OverlapAll"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetGenerateOverlapEvents(true);

}

void ABasicAmmoBox::OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATankPawn* PlayerTankPawn = Cast<ATankPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	ACannon* PlayerCannon = PlayerTankPawn->GetCannon();
	if (OtherActor == PlayerTankPawn)
	{
		bool Added = PlayerCannon->AddAmmo(Ammo);
		if (Added)
		{
			UE_LOG(LogTankogeddon, Warning, TEXT("Ammo added, current ammo: %d"), PlayerCannon->GetCurrentAmmo());
			Destroy();
		}
	}

}