// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Cannon.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageTaker.h"
#include "HealthComponent.h"
#include "Components/BoxComponent.h"
#include "TankPawn.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class ATankPlayerController;
class ACannon;
class UArrowComponent;

UCLASS()
class TANKOGEDDON_API ATankPawn : public APawn, public IDamageTaker
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	void AddCannon(TSubclassOf<ACannon> NewCannonClass = nullptr);


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		 UArrowComponent* CannonSetupPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		 UBoxComponent* HitCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* HittedAudioEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* DeathParticleEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* DeathAudioEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon")
		TSubclassOf<ACannon> CannonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UserInput|Cannons")
		TSubclassOf<ACannon> FirstCannonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UserInput|Cannons")
		TSubclassOf<ACannon> SecondCannonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UserInput|Cannons")
		TSubclassOf<ACannon> ThirdCannonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float MoveSpeed = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float RotationSpeed = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float MovementSmoothness = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float RotationSmoothness = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float TurretRotationSpeed = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon")
		bool bHasFirstCannon = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon")
		bool bHasSecondCannon = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret|Cannon")
		bool bHasThirdCannon = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Patrol points", Meta = (MakeEditWidget = true))
		TArray<FVector> PatrollingPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params|Accurency")
		float MovementAccurency = 50;

	int32 FirstCannonAmmo = -1.f;
	int32 SecondCannonAmmo = -1.f;
	int32 ThirdCannonAmmo = -1.f;

	FTimerHandle DeathTimerHandle;

public:
	// Sets default values for this pawn's properties
	ATankPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		void MoveForward(float AxisValue);
	UFUNCTION()
		void RotateRight(float AxisValue);
	UFUNCTION()
		void Fire();
	UFUNCTION()
		void AltFire();


	UFUNCTION()
		void TakeDamage(FDamageData DamageData);

	void SetupCannon(TSubclassOf<ACannon> SetupCannonClass);
	TSubclassOf<ACannon> GetCannonClass();
	ACannon* GetCannon();

	void ShowAmmoLeft();
	void SelectFirstCannon();
	void SelectSecondCannon();
	void SelectThirdCannon();

	UFUNCTION()
		TArray<FVector> GetPatrollingPoints() { return PatrollingPoints; };

	UFUNCTION()
		float GetMovementAccurency() { return MovementAccurency; };

	UFUNCTION()
		FVector GetTurretForwardVector();

	UFUNCTION()
		void RotateTurretTo(FVector TargetPosition);

	FVector GetEyesPosition();

private:
	float TargetForwardAxisValue = 0.f;
	float TargetRotateRightValue = 0.f;
	float CurrentForwardAxisValue = 0.f;
	float CurrentRightAxisValue = 0.f;

	bool isDead = false;


	UPROPERTY()
		ATankPlayerController* TankController;

	UPROPERTY()
		ACannon* Cannon;

	protected:
		UFUNCTION()
			void Die();

		UFUNCTION()
			void DamageTaken(float DamageValue);
			void DestroyTank();
};