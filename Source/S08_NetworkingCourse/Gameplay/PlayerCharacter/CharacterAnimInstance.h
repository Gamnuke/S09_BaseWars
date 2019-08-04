// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Gameplay/WeaponSystem/WeaponComponent.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class S08_NETWORKINGCOURSE_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	class AMainCharacter* CharacterRef;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Aiming;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Sprinting;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Falling;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool WeaponEquipped;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool FiringWeapon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Taunting;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool ReloadingWeapon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Speed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Direction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AimPitch;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWeaponSettings WeaponSettings;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector GripLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool FireAnimPlaying;

	//Changing Weapon
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int PreviousWeaponIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int CurrentWeaponIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool SwitchingWeapon;

public: // Weapon Variables
	UFUNCTION(BlueprintCallable)
		void AnimNotify_Fire();
	
	UFUNCTION(BlueprintCallable)
	void AnimNotify_HasSwitchedWeapon();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_HasFinishedSwitchingWeapon();

	UFUNCTION(BlueprintCallable)
		void AnimNotify_HasPutBackWeapon();

	UFUNCTION(BlueprintCallable)
		void AnimNotify_FinishedReloading();

};
