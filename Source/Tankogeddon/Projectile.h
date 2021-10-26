// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTaker.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"

UCLASS()

class TANKOGEDDON_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AProjectile();

	virtual void Start();
	virtual void Stop();
	void Explode();
	void MakeDamageOrAddForce(AActor* OtherActor);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float MoveSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
		float MoveRate = 0.005f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float Damage = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float PushForce = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		bool bExploding = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float ExplodeRadius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		UParticleSystem* ExplodeParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		USoundBase* ExplodeAudioEffect;

	FTimerHandle MovementTimerHandle;

	UFUNCTION()
		void OnMeshOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void Move();
};