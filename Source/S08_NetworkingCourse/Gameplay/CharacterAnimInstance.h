// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Gameplay/WeaponComponent.h"
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
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool Aiming;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool Sprinting;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool Falling;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool WeaponEquipped;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool FiringWeapon;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		float Speed;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		float Direction;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		float AimPitch;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FWeaponSettings WeaponSettings;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FVector GripLocation;

public: // Weapon Variables
	UFUNCTION(BlueprintCallable)
		void AnimNotify_Fire();
	
	
};
