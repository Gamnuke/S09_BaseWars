// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingObject.h"
#include "Engine/World.h"

// Sets default values
AMovingObject::AMovingObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMovingObject::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	};
	CStartLocation = GetActorLocation();
	CTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

// Called every frame
void AMovingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority()) {
		if (IsTriggered) {
			if (Moving) {
				CurrentTargetLocation = CTargetLocation;
			}
			else {
				CurrentTargetLocation = CStartLocation;
			}
			SetActorLocation(FMath::VInterpTo(GetActorLocation(), CurrentTargetLocation, DeltaTime, MoveSpeed));
		}
		else {
			SetActorLocation(FMath::Lerp(CStartLocation, CTargetLocation, (FMath::Sin(GetWorld()->GetTimeSeconds()))*0.5 + 0.5));
		}
	}
}

