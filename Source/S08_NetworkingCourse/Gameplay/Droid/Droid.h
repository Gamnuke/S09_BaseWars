// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Droid.generated.h"

USTRUCT(BlueprintType)
struct FDroidState {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Sprinting;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Attacking;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Moving;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class AMainCharacter *TargetCharacter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Dead = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Hit = false;

	
};

USTRUCT(BlueprintType)
struct FDroidSettings {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float WalkSpeed = 200;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RunSpeed = 390;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Damage = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Health = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AttackRange = 200;
};

UCLASS()
class S08_NETWORKINGCOURSE_API ADroid : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADroid();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class USpringArmComponent *CameraBoom;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UCameraComponent *CameraComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int CurrentHealth;

	FRotator LeadFlinchRotation;
	FRotator CurrentFlinchRotation;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OpenPauseMenu();
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable)
	void Attack();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	float DespawnTime;
	//UFUNCTION(Server, reliable, WithValidation)
	//	void SetServerState(FDroidState NewState);

	//UPROPERTY(ReplicatedUsing = OnRepServerState)
	//	FDroidState DroidServerState;

	//UFUNCTION()
	//void OnRepServerState();

	float TimeUntilStartRunning;

	FRotator MeshRotation;
	FRotator TargetRotation;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(NetMulticast, Reliable)
	void DamageDroid(int Amount, FHitResult Hit, AActiveGameState *InstigatorGameState);

	UFUNCTION(BlueprintCallable)
		void SetHit(bool NewHit);
	UFUNCTION(BlueprintCallable)
		void SetSprinting(bool Sprinting);
	UFUNCTION(BlueprintCallable)
		void SetAttacking(bool Attacking);
	UFUNCTION(BlueprintCallable)
		void SetTargetCharacter(class AMainCharacter * NewTargetCharacter);
public:
	UPROPERTY(replicated, BlueprintReadWrite, EditAnywhere)
	FDroidState DroidState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDroidSettings DroidSettings;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<class USoundBase*> AttackImpactSounds;
};
