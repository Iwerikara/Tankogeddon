// Fill out your copyright notice in the Description page of Project Settings.


#include "TankAIController.h"
#include "DrawDebugHelpers.h"
#include "MalchitsVS_UEBasics.h"

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void ATankAIController::Init()
{

	TankPawn = Cast<ATankPawn>(GetPawn());
	if (TankPawn)
	{
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		MovementAccurency = TankPawn->GetMovementAccurency();
		PatrollingPoints = TankPawn->GetPatrollingPoints();
		CurrentPatrolPointIndex = PatrollingPoints.Num() == 0 ? INDEX_NONE : 0;
	}
	
}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TankPawn)
	{
		Init();
	}

	if (!TankPawn)
	{
		return;
	}

	if (CurrentPatrolPointIndex == INDEX_NONE)
	{
		TankPawn->MoveForward(0.f);
		return;
	}

	TankPawn->MoveForward(1);
	float RotationValue = GetRotationValue();
	TankPawn->RotateRight(RotationValue);
	Targeting();
}

float ATankAIController::GetRotationValue()
{
	FVector CurrentPoint = PatrollingPoints[CurrentPatrolPointIndex];
	FVector PawnLocation = TankPawn->GetActorLocation();
	if (FVector::Distance(CurrentPoint, PawnLocation) <= MovementAccurency)
	{
		CurrentPatrolPointIndex++;
		if (CurrentPatrolPointIndex >= PatrollingPoints.Num())
		{
			CurrentPatrolPointIndex = 0;
		}
	}

	FVector MoveDirection = CurrentPoint - PawnLocation;
	MoveDirection.Normalize();
	FVector ForwardDirection = TankPawn->GetActorForwardVector();
	FVector RightDirection = TankPawn->GetActorRightVector();

	//DrawDebugLine(GetWorld(), pawnLocation, currentPoint, FColor::Blue, false, 0.1f, 0, 5);

	float ForwardAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardDirection, MoveDirection)));
	float RightAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightDirection, MoveDirection)));

	float RotationValue = 0.f;
	if (ForwardAngle > 5.f)
	{
		RotationValue = 1.f;
	}
	if (RightAngle > 90.f)
	{
		RotationValue = -RotationValue;
	}

	return RotationValue;
}

void ATankAIController::Targeting()
{
	if (CheckCanFire())
	{
		Fire();
	}
	else
	{
		RotateToPlayer();
	}
}

void ATankAIController::RotateToPlayer()
{
	if (IsPlayerInRange())
	{
		TankPawn->RotateTurretTo(PlayerPawn->GetActorLocation());
	}
}

bool ATankAIController::IsPlayerInRange()
{
	return FVector::Distance(TankPawn->GetActorLocation(), PlayerPawn->GetActorLocation()) <= TargetingRange;
}

bool ATankAIController::CheckCanFire()
{
	if (!CheckIsPlayerSeen())
	{
		return false;
	}

	FVector TargetingDir = TankPawn->GetTurretForwardVector();
	FVector DirToPlayer = PlayerPawn->GetActorLocation() - TankPawn->GetActorLocation();
	DirToPlayer.Normalize();
	float AimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDir, DirToPlayer)));

	return AimAngle <= Accurency;
}

void ATankAIController::Fire()
{
	TankPawn->Fire();
}

bool ATankAIController::CheckIsPlayerSeen()
{
	FVector PlayerPos = PlayerPawn->GetActorLocation();
	FVector EyesPos = TankPawn->GetEyesPosition();

	FHitResult HitResult;
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(TankPawn);
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, EyesPos, PlayerPos, ECollisionChannel::ECC_Visibility, TraceParams))
	{
		AActor* HitResultActor = HitResult.Actor.Get();
		if (HitResultActor)
		{
			DrawDebugLine(GetWorld(), EyesPos, HitResult.Location, FColor::Orange, false, 0.2f, 0, 10);
			//UE_LOG(LogTankogeddon, Warning, TEXT("Hit result: %s"), *HitResultActor->GetName());

			return HitResultActor == PlayerPawn;
		}
	}
	DrawDebugLine(GetWorld(), EyesPos, PlayerPos, FColor::Orange, false, 0.5f, 0, 10);

	return false;
}