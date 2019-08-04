// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/WeaponSystem/WeaponComponent.h"
#include "Projectile.generated.h"

UCLASS()
class S08_NETWORKINGCOURSE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	bool CanMove = true;
	bool OwnedByServer = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		struct FProjectileSettings ProjectileSettings;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UParticleSystemComponent *ProjectileTrail;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UParticleSystemComponent *ProjectileImpact;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UParticleSystemComponent *DroidImpact;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AActor> Trail;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USoundBase *DroidImpactSound;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<USoundBase*> WallImpactSounds;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USoundAttenuation *ImpactAttenuationSettings;


	class AActiveGameState *GamestateRef;

	class AActiveGameState *Instigator;
	UFUNCTION(BlueprintNativeEvent)
		void CallDestructionDelay();

	virtual void CallDestructionDelay_Implementation();


	float Speed = 10;

	FVector StartLocation;

	class UInGameHUD *HudRef;

	static USoundBase* AlterateSound(TArray<class USoundBase*> SoundAssets);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};