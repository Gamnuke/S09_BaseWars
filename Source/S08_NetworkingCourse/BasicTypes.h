#pragma once

#include "BasicTypes.generated.h"

UENUM(BlueprintType)
enum ESubCategory {
	// Components:
	Rotary, Cockpits,

	//Cosmetics:
	Lights,

	//Mobility:
	Arachnids, Hover, Pedal, Wheeled,

	// Structure:
	Armoured, Light, Medium, Shielded,

	// Weaponry:
	Barrels, Enhancers, Muzzles, WModules
};

USTRUCT(Blueprintable)
struct FComplexInt32 {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FVector2D Limit = FVector2D(0, 100);
};
USTRUCT(Blueprintable)
struct FComplexFloat {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) float Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FVector2D Limit = FVector2D(0.0f, 100.0f);
};
USTRUCT(Blueprintable)
struct FComplexBool {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool Value;
};
USTRUCT(Blueprintable)
struct FComplexString {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FString Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 CharacterLimit = 100;
};

USTRUCT(Blueprintable)
struct FUnmodifiableVariables {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Health;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Mass;

	//Wheel
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Traction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Radius;
};

USTRUCT(Blueprintable)
struct FModifiableVariables {
	GENERATED_USTRUCT_BODY();
	//Wheel
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Speed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Acceleration;


	//Rotary Module
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 RotationSpeed;
};

USTRUCT(Blueprintable)
struct FPartStats {
	GENERATED_USTRUCT_BODY();
	///Editor Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) TEnumAsByte<ESubCategory> Category;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 Cost;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bLockedByDefault = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bNeedsRoot;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bModifiable;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bUsesPhysics;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) class USkeletalMesh * SkeletalMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) class UStaticMesh * StaticMesh;


	///Unmodifiable variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = UnModifiableSettings)
		FUnmodifiableVariables StaticVariables = FUnmodifiableVariables();

	///Modifiable Variables
		//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = ModifiableSettings)
		FModifiableVariables ModifiableVariables = FModifiableVariables();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) struct FModifiableVariables ModifiableVariables = FModifiableVariables();
	/*class USkeletalPartMesh * SkeletalMeshReference;
	class UStaticPartMesh *StaticMeshReference;*/

	/*FString GetText() {
		if (UseDamage) { DetailText += FString("Damage: ") + FString::FromInt(Damage) + FString("\n"); }
		if (UseFireRate) { DetailText += FString("Fire Rate: ") + FString::SanitizeFloat(FireRate) + FString("\n"); }
		if (UseSpeed) { DetailText += FString("Speed: ") + FString::SanitizeFloat(Speed) + FString("\n"); }
		return DetailText;
	}*/
};