// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Cannon.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageTaker.h"
#include "HealthComponent.h"
#include "Components/BoxComponent.h"
#include "TankPawn.generated.h"

class ATankPlayerController;
class ACannon;

UCLASS()
class TANKOGEDDON_API ATankPawn : public APawn, public IDamageTaker
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	void AddCannon(TSubclassOf<ACannon> NewCannonClass = nullptr);


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class	UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UArrowComponent* CannonSetupPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UBoxComponent* HitCollider;

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


	void SelectFirstCannon();
	void SelectSecondCannon();
	void SelectThirdCannon();

private:
	float TargetForwardAxisValue = 0.f;
	float TargetRotateRightValue = 0.f;
	float CurrentForwardAxisValue = 0.f;
	float CurrentRightAxisValue = 0.f;


	UPROPERTY()
		ATankPlayerController* TankController;

	UPROPERTY()
		ACannon* Cannon;

	protected:
		UFUNCTION()
			void Die();

		UFUNCTION()
			void DamageTaken(float DamageValue);
};