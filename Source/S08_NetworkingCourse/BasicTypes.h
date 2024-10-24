#pragma once

#include "Serialization/Archive.h"

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FString VariableName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FVector2D Limit = FVector2D(0, 100);

	FComplexInt32(){}
	FComplexInt32(const FString &NewName, const int32 &NewValue , const FVector2D &NewLimit) : VariableName(NewName), Value(NewValue), Limit(NewLimit) {
		
	}

	void ChangeValue(int32 NewValue) {
		Value = FMath::Clamp(NewValue, FMath::RoundToInt(Limit.X), FMath::RoundToInt(Limit.Y));
		
	}
};
USTRUCT(Blueprintable)
struct FComplexFloat {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FString VariableName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) float Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FVector2D Limit = FVector2D(0.0f, 100.0f);
	FComplexFloat() {}
	FComplexFloat(const FString &NewName, const float &NewValue, const FVector2D &NewLimit) : VariableName(NewName), Value(NewValue), Limit(NewLimit) {

	}

};
USTRUCT(Blueprintable)
struct FComplexString {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FString VariableName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FString Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 CharacterLimit = 100;
	FComplexString() {}
	FComplexString(const FString &NewName, const FString &NewValue, const int32 &NewLimit) : VariableName(NewName), Value(NewValue), CharacterLimit(NewLimit) {

	}

	

};

USTRUCT(Blueprintable)
struct FUnmodifiableVariables {
	GENERATED_USTRUCT_BODY();
	//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Health = FComplexInt32("Health", 20, FVector2D::ZeroVector);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Mass = FComplexInt32("Mass", 20, FVector2D::ZeroVector);

	//Wheel
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Traction = FComplexInt32("Traction", 20, FVector2D::ZeroVector);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Radius = FComplexInt32("Radius", 20, FVector2D::ZeroVector);

	FUnmodifiableVariables() {};

};

USTRUCT(Blueprintable)
struct FModifiableVariables {
	GENERATED_USTRUCT_BODY();
	//Wheel
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Speed = FComplexInt32("Speed", 20, FVector2D::ZeroVector);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 Acceleration = FComplexInt32("Acceleration", 20, FVector2D::ZeroVector);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 SteeringAngle = FComplexInt32("Steering Angle", 30, FVector2D(0,50));
	
	//Rotary Module
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 PositionStrength = FComplexInt32("Position Strength", 50000, FVector2D(0,200000));
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 VelocityStrength = FComplexInt32("Velocity Strength", 20000, FVector2D(0, 200000));
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) FComplexInt32 MaxForce = FComplexInt32("Max Force", 20, FVector2D(0, 999999999));
	FModifiableVariables() {};
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FComplexInt32 &SaveStatsData) { Ar << SaveStatsData.Value; Ar << SaveStatsData.Limit; return Ar; }
FORCEINLINE FArchive& operator<<(FArchive &Ar, FComplexFloat &SaveStatsData) { Ar << SaveStatsData.Value; Ar << SaveStatsData.Limit; return Ar; }
FORCEINLINE FArchive& operator<<(FArchive &Ar, FComplexString &SaveStatsData) { Ar << SaveStatsData.Value; Ar << SaveStatsData.CharacterLimit; return Ar; }
FORCEINLINE FArchive& operator<<(FArchive &Ar, FModifiableVariables &SaveStatsData) {
	Ar << SaveStatsData.Speed;
	Ar << SaveStatsData.Acceleration;
	Ar << SaveStatsData.PositionStrength;
	Ar << SaveStatsData.VelocityStrength;
	Ar << SaveStatsData.MaxForce;
	return Ar;
}
FORCEINLINE FArchive& operator<<(FArchive &Ar, FUnmodifiableVariables &SaveStatsData) {
	Ar << SaveStatsData.Health;
	Ar << SaveStatsData.Mass;
	Ar << SaveStatsData.Traction; 
	Ar << SaveStatsData.Radius; 
	return Ar;
}

USTRUCT(Blueprintable)
struct FPartStats {
	GENERATED_USTRUCT_BODY();
	///Editor Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) TEnumAsByte<ESubCategory> Category;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) int32 Cost;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bLockedByDefault = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bModifiable;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) bool bUsesPhysics;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) class USkeletalMesh * TheSkeletalMesh = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) class UStaticMesh * StaticMesh = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Main) TSubclassOf<class UAnimInstance> AnimInstance = nullptr;

	//Note: Anything that uses physics will need a root part.
	///Unmodifiable variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = UnModifiableSettings)
		FUnmodifiableVariables StaticVariables = FUnmodifiableVariables();

	///Modifiable Variables
		//Basic
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = ModifiableSettings)
		FModifiableVariables ModifiableVariables = FModifiableVariables();
	
	UPROPERTY()
		FVector PartLocation;
	UPROPERTY()
		FRotator PartRotation;

	FString NameTest = "Not Overrided";
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

FORCEINLINE FArchive& operator<<(FArchive &Ar, FPartStats &SaveStatsData) {
	Ar << SaveStatsData.PartLocation;
	Ar << SaveStatsData.PartRotation;
	Ar << SaveStatsData.ModifiableVariables;
	Ar << SaveStatsData.StaticVariables;
	Ar << SaveStatsData.NameTest;
	return Ar;
}


